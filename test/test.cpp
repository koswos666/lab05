#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"

using ::testing::_;
using ::testing::Return;
using ::testing::Throw;
using ::testing::StrictMock;

// Мок-класс для Account
class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

// Мок-класс для Transaction
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
    Account acc3(2, 300); // Фиксированный аккаунт для тестов
    
    EXPECT_THROW(tr.Make(acc1, acc2, 100), std::logic_error);
    EXPECT_THROW(tr.Make(acc1, acc3, -50), std::invalid_argument);
    EXPECT_THROW(tr.Make(acc1, acc3, 99), std::logic_error);
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
    EXPECT_FALSE(tr.Make(from, to, 100));
}

TEST(TransactionTest, MakeSuccessfulTransaction) {
    StrictMock<MockAccount> from(1, 200);
    StrictMock<MockAccount> to(2, 0);
    MockTransaction tr;
    
    EXPECT_CALL(from, Lock());
    EXPECT_CALL(to, Lock());
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(200));
    EXPECT_CALL(tr, SaveToDataBase(_, _, 100));
    EXPECT_CALL(to, ChangeBalance(100));
    EXPECT_CALL(from, ChangeBalance(-101));
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

// Тестирование приватных методов
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
    
    TransactionTestFriend::Credit(tr, acc, 50);
    EXPECT_EQ(acc.GetBalance(), 150);
    
    EXPECT_TRUE(TransactionTestFriend::Debit(tr, acc, 50));
    EXPECT_EQ(acc.GetBalance(), 100);
    
    EXPECT_FALSE(TransactionTestFriend::Debit(tr, acc, 150));
    
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
