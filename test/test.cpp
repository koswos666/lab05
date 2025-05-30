#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"
#include <sstream> 

using namespace testing;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

TEST(TransactionTest, FullTransferFlow) {
    MockAccount from(1, 2000);
    MockAccount to(2, 500);
    Transaction tr;

    // Установка ожиданий
    {
        InSequence seq;
        
        // Проверка блокировки
        EXPECT_CALL(from, Lock());
        EXPECT_CALL(to, Lock());
        
        // Проверка изменения баланса
        EXPECT_CALL(from, GetBalance()).WillOnce(Return(2000));
        EXPECT_CALL(from, ChangeBalance(-301));
        EXPECT_CALL(to, ChangeBalance(300));
        
        // Проверка разблокировки
        EXPECT_CALL(to, Unlock());
        EXPECT_CALL(from, Unlock());
    }

    ASSERT_TRUE(tr.Make(from, to, 300));
}

TEST(TransactionTest, InvalidTransfers) {
    MockAccount acc1(1, 100), acc2(2, 200);
    Transaction tr;

    // Перевод на тот же счет
    ASSERT_THROW(tr.Make(acc1, acc1, 200), std::logic_error);
    
    // Отрицательная сумма
    ASSERT_THROW(tr.Make(acc1, acc2, -50), std::invalid_argument);
    
    // Слишком маленькая сумма
    ASSERT_THROW(tr.Make(acc1, acc2, 99), std::logic_error);
    
    // Комиссия слишком большая
    ASSERT_FALSE(tr.Make(acc1, acc2, 1));
}

TEST(TransactionTest, InsufficientBalanceHandling) {
    MockAccount from(1, 100);
    MockAccount to(2, 0);
    Transaction tr;

    {
        InSequence seq;
        EXPECT_CALL(from, Lock());
        EXPECT_CALL(to, Lock());
        EXPECT_CALL(from, GetBalance()).WillOnce(Return(100));
        EXPECT_CALL(to, Unlock());
        EXPECT_CALL(from, Unlock());
    }

    ASSERT_FALSE(tr.Make(from, to, 150));
}

TEST(TransactionTest, SaveToDatabaseAndFee) {
    Account from(1, 2000);
    Account to(2, 500);
    Transaction tr;

    // Проверка вывода в базу данных
    std::streambuf* old = std::cout.rdbuf();
    std::stringstream buffer;
    std::cout.rdbuf(buffer.rdbuf());

    ASSERT_TRUE(tr.Make(from, to, 300));
    
    std::cout.rdbuf(old);

    std::string output = buffer.str();
    EXPECT_THAT(output, HasSubstr("1 send to 2 $300"));
    EXPECT_THAT(output, HasSubstr("Balance 1 is 1699"));
    EXPECT_THAT(output, HasSubstr("Balance 2 is 800"));

    // Проверка работы с комиссией
    tr.set_fee(5);
    ASSERT_EQ(tr.fee(), 5);
    
    // Проверка пограничного значения комиссии
    Account from2(1, 500);
    Account to2(2, 0);
    tr.set_fee(50);
    ASSERT_TRUE(tr.Make(from2, to2, 100));
    ASSERT_EQ(from2.GetBalance(), 350);
    ASSERT_EQ(to2.GetBalance(), 100);
}
