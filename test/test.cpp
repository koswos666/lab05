#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"

using ::testing::_;
using ::testing::Throw;

class TransactionTestFriend : public Transaction {
public:
    void TestCredit(Account& acc, int sum) { Credit(acc, sum); }
    bool TestDebit(Account& acc, int sum) { return Debit(acc, sum); }
};

TEST(AccountTest, ConstructorAndGetters) {
    Account acc(1, 500);
    ASSERT_EQ(acc.GetBalance(), 500);
    ASSERT_EQ(acc.id(), 1);
}

TEST(AccountTest, LockUnlock) {
    Account acc(2, 200);
    ASSERT_NO_THROW(acc.Lock());
    ASSERT_THROW(acc.Lock(), std::runtime_error);
    acc.Unlock();
    ASSERT_NO_THROW(acc.Lock());
}

TEST(AccountTest, ChangeBalance) {
    Account acc(3, 300);
    
    ASSERT_THROW(acc.ChangeBalance(100), std::runtime_error);
    
    acc.Lock();
    ASSERT_NO_THROW(acc.ChangeBalance(-200));
    ASSERT_EQ(acc.GetBalance(), 100);
    
    acc.Unlock();
    ASSERT_THROW(acc.ChangeBalance(-100), std::runtime_error);
}

TEST(TransactionTest, FullCoverage) {
    TransactionTestFriend tr;
    Account acc1(1, 1000);
    Account acc2(2, 500);

    ASSERT_THROW(tr.Make(acc1, acc1, 100), std::logic_error);
    ASSERT_THROW(tr.Make(acc1, acc2, -50), std::invalid_argument);
    ASSERT_THROW(tr.Make(acc1, acc2, 99), std::logic_error);

    Account acc3(3, 500);
    ASSERT_FALSE(tr.Make(acc3, acc2, 499));

    ASSERT_TRUE(tr.Make(acc1, acc2, 400));
    ASSERT_EQ(acc1.GetBalance(), 599);
    ASSERT_EQ(acc2.GetBalance(), 900);
}

TEST(TransactionTest, CreditDebitEdgeCases) {
    TransactionTestFriend tr;
    Account acc(1, 100);
    acc.Lock();

    ASSERT_DEATH(tr.TestCredit(acc, 0), ".*");
    ASSERT_TRUE(tr.TestDebit(acc, 50));
    ASSERT_FALSE(tr.TestDebit(acc, 100));
}
