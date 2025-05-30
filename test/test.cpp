#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"

using namespace testing;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

TEST(TransactionTest, SuccessfulTransferBetweenDifferentAccounts) {
    MockAccount from(1, 2000);
    MockAccount to(2, 500);
    Transaction tr;

    testing::InSequence seq;

    EXPECT_CALL(from, Lock());
    EXPECT_CALL(to, Lock());
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(2000));
    EXPECT_CALL(from, ChangeBalance(-301));
    EXPECT_CALL(to, ChangeBalance(300));
    EXPECT_CALL(to, Unlock());
    EXPECT_CALL(from, Unlock());
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(1699));
    EXPECT_CALL(to, GetBalance()).WillOnce(Return(800));   

    ASSERT_TRUE(tr.Make(from, to, 300));
}

TEST(TransactionTest, TransferToSameAccountFails) {
    MockAccount acc(1, 100);
    Transaction tr;
    ASSERT_THROW(tr.Make(acc, acc, 200), std::logic_error);
}

TEST(TransactionTest, NegativeAmountTransferFails) {
    MockAccount from(1, 200);
    MockAccount to(2, 100);
    Transaction tr;
    ASSERT_THROW(tr.Make(from, to, -50), std::invalid_argument);
}

TEST(TransactionTest, SmallAmountTransferFails) {
    MockAccount from(1, 200);
    MockAccount to(2, 100);
    Transaction tr;
    ASSERT_THROW(tr.Make(from, to, 99), std::logic_error);
}

TEST(TransactionTest, FeeTooLargeCancelsTransfer) {
    MockAccount from(1, 300);
    MockAccount to(2, 0);
    Transaction tr;

    EXPECT_CALL(from, Lock()).Times(0);
    EXPECT_CALL(to, Lock()).Times(0);
    ASSERT_FALSE(tr.Make(from, to, 1)); 
}

TEST(TransactionTest, InsufficientBalanceCancelsTransfer) {
    MockAccount from(1, 100);
    MockAccount to(2, 0);
    Transaction tr;

    EXPECT_CALL(from, Lock());
    EXPECT_CALL(to, Lock());
    EXPECT_CALL(to, Unlock());
    EXPECT_CALL(from, Unlock());
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(100));
    ASSERT_FALSE(tr.Make(from, to, 150)); 
}

TEST(TransactionTest, UnlockOrderMaintainedOnFailure) {
    MockAccount from(1, 50);
    MockAccount to(2, 0);
    Transaction tr;

    testing::InSequence seq;
    EXPECT_CALL(from, Lock());
    EXPECT_CALL(to, Lock());
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(50));
    EXPECT_CALL(to, Unlock());
    EXPECT_CALL(from, Unlock());

    ASSERT_FALSE(tr.Make(from, to, 100));
}
