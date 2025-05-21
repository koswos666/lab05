#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"

using ::testing::_;
using ::testing::Throw;
using ::testing::NiceMock;

class MockTransaction : public Transaction {
public:
    MOCK_METHOD(void, SaveToDataBase, (Account&, Account&, int), (override));
};

TEST(AccountTest, GetBalance) {
    Account acc(1, 100);
    ASSERT_EQ(acc.GetBalance(), 100);
}

TEST(AccountTest, LockUnlock) {
    Account acc(1, 200);
    ASSERT_NO_THROW(acc.Lock());
    ASSERT_THROW(acc.Lock(), std::runtime_error);
    acc.Unlock();
    ASSERT_NO_THROW(acc.Lock());
}

TEST(AccountTest, ChangeBalance) {
    Account acc(1, 100);
    ASSERT_THROW(acc.ChangeBalance(50), std::runtime_error);
    acc.Lock();
    ASSERT_NO_THROW(acc.ChangeBalance(50));
    ASSERT_EQ(acc.GetBalance(), 150);
    acc.Unlock();
    ASSERT_THROW(acc.ChangeBalance(-200), std::runtime_error);
}

TEST(TransactionTest, MakeValidTransaction) {
    NiceMock<MockTransaction> transaction;
    Account acc_from(1, 500);
    Account acc_to(2, 100);
    
    EXPECT_CALL(transaction, SaveToDataBase(_, _, 100)).Times(1);
    ASSERT_TRUE(transaction.Make(acc_from, acc_to, 100));
    ASSERT_EQ(acc_from.GetBalance(), 399);
    ASSERT_EQ(acc_to.GetBalance(), 200);
}

TEST(TransactionTest, InvalidTransactions) {
    NiceMock<MockTransaction> transaction;
    Account acc1(1, 100);
    Account acc2(2, 200);
    
    ASSERT_THROW(transaction.Make(acc1, acc2, -50), std::invalid_argument);
    
    ASSERT_THROW(transaction.Make(acc1, acc1, 100), std::logic_error);
    
    ASSERT_THROW(transaction.Make(acc1, acc2, 99), std::logic_error);
}

TEST(TransactionTest, InsufficientFunds) {
    NiceMock<MockTransaction> transaction;
    Account acc_from(1, 50);
    Account acc_to(2, 100);
    
    EXPECT_CALL(transaction, SaveToDataBase(_, _, _)).Times(0);
    ASSERT_FALSE(transaction.Make(acc_from, acc_to, 100));
}

TEST(TransactionTest, DebitFailure) {
    Transaction transaction;
    Account acc(1, 50);
    acc.Lock();
  
    ASSERT_FALSE(transaction.Debit(acc, 100));
    ASSERT_EQ(acc.GetBalance(), 50);
}

TEST(TransactionTest, DatabaseErrorHandling) {
    MockTransaction transaction;
    Account acc_from(1, 500);
    Account acc_to(2, 100);
    
    EXPECT_CALL(transaction, SaveToDataBase(_, _, 100))
        .WillOnce(Throw(std::runtime_error("DB error")));
    
    ASSERT_FALSE(transaction.Make(acc_from, acc_to, 100));
    ASSERT_EQ(acc_from.GetBalance(), 500);
    ASSERT_EQ(acc_to.GetBalance(), 100);
}

TEST(TransactionTest, CreditDebitOperations) {
    Transaction transaction;
    Account acc(1, 100);
    acc.Lock();
    
    transaction.Credit(acc, 50);
    ASSERT_EQ(acc.GetBalance(), 150);

    ASSERT_TRUE(transaction.Debit(acc, 50));
    ASSERT_EQ(acc.GetBalance(), 100);
    
    ASSERT_FALSE(transaction.Debit(acc, 150));
}
