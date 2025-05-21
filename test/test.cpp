#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"

using ::testing::_;
using ::testing::Return;
using ::testing::Throw;
using ::testing::NiceMock;

class MockTransaction : public Transaction {
protected:
    MOCK_METHOD(void, SaveToDataBase, (Account&, Account&, int), (override));
};

TEST(AccountTest, GetBalance) {
    Account acc(1, 100);
    ASSERT_EQ(acc.GetBalance(), 100);
}

TEST(AccountTest, ChangeBalance) {
    Account acc(1, 100);
    ASSERT_THROW(acc.ChangeBalance(50), std::runtime_error);
    acc.Lock();
    acc.ChangeBalance(50);
    ASSERT_EQ(acc.GetBalance(), 150);
    acc.Unlock();
}

TEST(TransactionTest, MakeTransaction) {
    NiceMock<MockTransaction> transaction;
    Account acc_from(1, 500);
    Account acc_to(2, 100);
    
    
    ASSERT_THROW(transaction.Make(acc_from, acc_to, -50), std::invalid_argument);
    
   
    ASSERT_THROW(transaction.Make(acc_from, acc_from, 100), std::logic_error);
    
 
    EXPECT_CALL(transaction, SaveToDataBase(_, _, 100))
        .Times(1);
    
    ASSERT_TRUE(transaction.Make(acc_from, acc_to, 100));
    ASSERT_EQ(acc_from.GetBalance(), 399);  
    ASSERT_EQ(acc_to.GetBalance(), 200);   
}

TEST(TransactionTest, FailedTransactionNotSaved) {
    NiceMock<MockTransaction> transaction;
    Account acc_from(1, 50); 
    Account acc_to(2, 100);
    
    EXPECT_CALL(transaction, SaveToDataBase(_, _, _))
        .Times(0);
    
    ASSERT_FALSE(transaction.Make(acc_from, acc_to, 100));
}

TEST(TransactionTest, DatabaseSaveFailure) {
    MockTransaction transaction;
    Account acc_from(1, 500);
    Account acc_to(2, 100);
    
    EXPECT_CALL(transaction, SaveToDataBase(_, _, 100))
        .WillOnce(Throw(std::runtime_error("DB error")));
    
    ASSERT_FALSE(transaction.Make(acc_from, acc_to, 100));
    
    ASSERT_EQ(acc_from.GetBalance(), 500);
    ASSERT_EQ(acc_to.GetBalance(), 100);
}
