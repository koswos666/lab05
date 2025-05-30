#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"

using ::testing::_;
using ::testing::Return;
using ::testing::Throw;
using ::testing::StrictMock;
using ::testing::AtLeast;


class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

// Мок-класс для Transaction (для перехвата SaveToDataBase)
class MockTransaction : public Transaction {
public:
    MOCK_METHOD(void, SaveToDataBase, (Account& from, Account& to, int sum), (override));
};

// Тесты для класса Account
TEST(AccountTest, LockUnlockBehavior) {
    Account acc(1, 100);
    
    acc.Lock();
    EXPECT_NO_THROW(acc.ChangeBalance(50));
    EXPECT_EQ(acc.GetBalance(), 150);
    
    acc.Unlock();
    EXPECT_THROW(acc.ChangeBalance(50), std::runtime_error);
}

TEST(AccountTest, LockThrowsWhenAlreadyLocked) {
    Account acc(1, 100);
    acc.Lock();
    EXPECT_THROW(acc.Lock(), std::runtime_error);
}

// Тесты для класса Transaction
TEST(TransactionTest, MakeFailsOnInvalidAccounts) {
    Transaction tr;
    Account acc1(1, 100), acc2(1, 200);
    
    EXPECT_THROW(tr.Make(acc1, acc2, 100), std::logic_error); // Same ID
    EXPECT_THROW(tr.Make(acc1, Account(2, 200), -50), std::invalid_argument); // Negative sum
    EXPECT_THROW(tr.Make(acc1, Account(2, 200), 99), std::logic_error); // Sum < 100
}

TEST(TransactionTest, MakeFailsWhenInsufficientFunds) {
    StrictMock<MockAccount> from(1, 100);
    StrictMock<MockAccount> to(2, 0);
    
    EXPECT_CALL(from, Lock());
    EXPECT_CALL(to, Lock());
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(100));
    EXPECT_CALL(from, Unlock());
    EXPECT_CALL(to, Unlock());
    
    Transaction tr;
    EXPECT_FALSE(tr.Make(from, to, 100)); // Нужно 100 + 1(fee) = 101
}

TEST(TransactionTest, MakeSuccessfulTransaction) {
    StrictMock<MockAccount> from(1, 200);
    StrictMock<MockAccount> to(2, 0);
    MockTransaction tr;
    
    // Ожидаемые вызовы
    EXPECT_CALL(from, Lock());
    EXPECT_CALL(to, Lock());
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(200));
    EXPECT_CALL(tr, SaveToDataBase(_, _, 100));
    EXPECT_CALL(to, ChangeBalance(100)); // Зачисление
    EXPECT_CALL(from, ChangeBalance(-101)); // Списание + комиссия
    EXPECT_CALL(from, Unlock());
    EXPECT_CALL(to, Unlock());
    
    EXPECT_TRUE(tr.Make(from, to, 100));
}

TEST(TransactionTest, MakeFailsWhenDatabaseThrows) {
    StrictMock<MockAccount> from(1, 200);
    StrictMock<MockAccount> to(2, 0);
    MockTransaction tr;
    
    EXPECT_CALL(from, Lock());
    EXPECT_CALL(to, Lock());
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(200));
    EXPECT_CALL(tr, SaveToDataBase(_, _, _)).WillOnce(Throw(std::runtime_error("DB Error")));
    EXPECT_CALL(from, Unlock());
    EXPECT_CALL(to, Unlock());
    
    EXPECT_FALSE(tr.Make(from, to, 100));
}

// Тестирование приватных методов через дружественный класс
class TransactionTestFriend {
public:
    static void Credit(Transaction& tr, Account& acc, int sum) {
        tr.Credit(acc, sum);
    }
    static bool Debit(Transaction& tr, Account& acc, int sum) {
        return tr.Debit(acc, sum);
    }
};

TEST(TransactionTest, CreditAndDebitOperations) {
    Transaction tr;
    Account acc(1, 100);
    acc.Lock();
    
    // Тест Credit
    TransactionTestFriend::Credit(tr, acc, 50);
    EXPECT_EQ(acc.GetBalance(), 150);
    
    // Тест Debit (успех)
    EXPECT_TRUE(TransactionTestFriend::Debit(tr, acc, 50));
    EXPECT_EQ(acc.GetBalance(), 100);
    
    // Тест Debit (недостаточно средств)
    EXPECT_FALSE(TransactionTestFriend::Debit(tr, acc, 150));
    
    // Тест исключений
    EXPECT_THROW(TransactionTestFriend::Credit(tr, acc, -10), std::invalid_argument);
    EXPECT_THROW(TransactionTestFriend::Debit(tr, acc, -10), std::invalid_argument);
}

TEST(TransactionTest, FeeConfiguration) {
    Transaction tr;
    tr.set_fee(10);
    EXPECT_EQ(tr.fee(), 10);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
