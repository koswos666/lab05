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

TEST(AccountTest, ConstructorAndMethods) {
    Account acc(1, 500);
    ASSERT_EQ(acc.GetBalance(), 500);
    ASSERT_EQ(acc.id(), 1);

   
    ASSERT_NO_THROW(acc.Lock());
    ASSERT_THROW(acc.Lock(), std::runtime_error); 
    acc.Unlock();
    ASSERT_NO_THROW(acc.Lock());
}

TEST(AccountTest, ChangeBalanceEdgeCases) {
    Account acc(2, 200);
    

    ASSERT_THROW(acc.ChangeBalance(100), std::runtime_error);
    
    acc.Lock();

    ASSERT_NO_THROW(acc.ChangeBalance(-50));
    ASSERT_EQ(acc.GetBalance(), 150);
    
    ASSERT_NO_THROW(acc.ChangeBalance(-200));
    ASSERT_EQ(acc.GetBalance(), -50);
}

TEST(TransactionTest, FullCoverage) {
    TransactionTestFriend tr;
    Account acc1(1, 1000);
    Account acc2(2, 500);


    ASSERT_THROW(tr.Make(acc1, acc1, 100), std::logic_error); // Same account
    ASSERT_THROW(tr.Make(acc1, acc2, -50), std::invalid_argument); // Negative sum
    ASSERT_THROW(tr.Make(acc1, acc2, 99), std::logic_error); // Sum < 100

 
    ASSERT_FALSE(tr.Make(acc1, acc2, 950)); // 1000 - (950 + 1) = 49 < 0


    ASSERT_TRUE(tr.Make(acc1, acc2, 500));
    ASSERT_EQ(acc1.GetBalance(), 499); // 1000 - 500 - 1
    ASSERT_EQ(acc2.GetBalance(), 1000);
}

TEST(TransactionTest, CreditDebitChecks) {
    TransactionTestFriend tr;
    Account acc(1, 100);
    acc.Lock();

   
    tr.TestCredit(acc, 50);
    ASSERT_EQ(acc.GetBalance(), 150);

    ASSERT_TRUE(tr.TestDebit(acc, 50));
    ASSERT_EQ(acc.GetBalance(), 100);

    ASSERT_FALSE(tr.TestDebit(acc, 150));
}
