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
    Account acc1(1, 999);
    Account acc2(2, 500);

   
    ASSERT_THROW(tr.Make(acc1, acc1, 100), std::logic_error);
 
    ASSERT_NO_THROW(acc1.Lock()); 
    acc1.Unlock(); 
    
    ASSERT_THROW(tr.Make(acc1, acc2, -50), std::invalid_argument);
    ASSERT_NO_THROW(acc1.Lock());
    acc1.Unlock();
   

    
    ASSERT_THROW(tr.Make(acc1, acc2, 99), std::logic_error);
    ASSERT_NO_THROW(acc1.Lock());
    acc1.Unlock();
    

  
    ASSERT_FALSE(tr.Make(acc1, acc2, 999));
   
    ASSERT_NO_THROW(acc1.Lock());
    acc1.Unlock();
    

  
    acc1.Lock();
    acc1.ChangeBalance(1);
    acc1.Unlock();

 
    ASSERT_TRUE(tr.Make(acc1, acc2, 500));
    ASSERT_EQ(acc1.GetBalance(), 499); 
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

TEST(TransactionTest, SaveToDatabaseException) {
    Transaction tr;
    Account acc1(1, 1000);
    Account acc2(2, 500);
    

    class MockTransaction : public Transaction {
    protected:
        void SaveToDataBase(Account& from, Account& to, int sum) override {
            throw std::runtime_error("DB error");
        }
    };

    MockTransaction mockTr;
    ASSERT_FALSE(mockTr.Make(acc1, acc2, 100));
}

TEST(TransactionTest, SumBoundary) {
    Transaction tr;
    Account acc1(1, 200);
    Account acc2(2, 0);

    ASSERT_TRUE(tr.Make(acc1, acc2, 100));
    ASSERT_EQ(acc1.GetBalance(), 99); 
    ASSERT_EQ(acc2.GetBalance(), 100);
}

TEST(TransactionTest, DebitFailure) {
    TransactionTestFriend tr;
    Account acc(1, 50);
    acc.Lock();
    
    ASSERT_FALSE(tr.TestDebit(acc, 100));
    ASSERT_EQ(acc.GetBalance(), 50);
}

TEST(TransactionTest, GuardUnlocksOnException) {
    Transaction tr;
    Account acc1(1, 1000);
    Account acc2(2, 500);

    try {
        tr.Make(acc1, acc2, 99);
    } catch (...) {}

    ASSERT_NO_THROW(acc1.Lock()); 
    ASSERT_NO_THROW(acc2.Lock());
}

TEST(TransactionTest, FeeConfiguration) {
    Transaction tr;
    tr.set_fee(10);
    ASSERT_EQ(tr.fee(), 10);
}


TEST(TransactionTest, DebitInsufficientBalance) {
    TransactionTestFriend tr;
    Account acc(1, 50);
    acc.Lock();
    
    ASSERT_FALSE(tr.TestDebit(acc, 100));
    ASSERT_EQ(acc.GetBalance(), 50);
}

TEST(AccountTest, ChangeBalanceWithoutLock) {
    Account acc(1, 200);
    ASSERT_THROW(acc.ChangeBalance(100), std::runtime_error); 
}

TEST(TransactionTest, InsufficientBalance) {
    Transaction tr;
    Account acc1(1, 100);
    Account acc2(2, 0);

    ASSERT_FALSE(tr.Make(acc1, acc2, 100));
}

TEST(TransactionTest, DatabaseExceptionHandling) {
    class MockTransaction : public Transaction {
    protected:
        void SaveToDataBase(Account& from, Account& to, int sum) override {
            throw std::runtime_error("DB error");
        }
    };

    MockTransaction mockTr;
    Account acc1(1, 1000);
    Account acc2(2, 500);

    ASSERT_FALSE(mockTr.Make(acc1, acc2, 100));
}
TEST(AccountTest, DoubleLockThrows) {
    Account acc(1, 100);
    acc.Lock();
    ASSERT_THROW(acc.Lock(), std::runtime_error);
}
TEST(TransactionTest, SameAccountIdsThrow) {
    Transaction tr;
    Account acc(1, 500);
    ASSERT_THROW(tr.Make(acc, acc, 100), std::logic_error); 
}
TEST(TransactionTest, SmallSumThrow) {
    Transaction tr;
    Account acc1(1, 200);
    Account acc2(2, 0);
    ASSERT_THROW(tr.Make(acc1, acc2, 99), std::logic_error); 
}

TEST(TransactionTest, DebitFailsWhenLowBalance) {
    TransactionTestFriend tr;
    Account acc(1, 50);
    acc.Lock();
    ASSERT_FALSE(tr.TestDebit(acc, 100)); 
    ASSERT_EQ(acc.GetBalance(), 50);
}
TEST(TransactionTest, CreditZeroSumThrows) {
    TransactionTestFriend tr;
    Account acc(1, 100);
    acc.Lock();
    ASSERT_THROW(tr.TestCredit(acc, 0), std::invalid_argument);
}


TEST(TransactionTest, SaveToDatabaseCalledCorrectly) {
    class MockTransaction : public Transaction {
    protected:
        void SaveToDataBase(Account& from, Account& to, int sum) override {
            called = true;
            Transaction::SaveToDataBase(from, to, sum);
        }
    public:
        bool called = false;
    };

    MockTransaction mockTr;
    Account acc1(1, 1000);
    Account acc2(2, 500);
    
    ASSERT_TRUE(mockTr.Make(acc1, acc2, 100));
    ASSERT_TRUE(mockTr.called);
}
TEST(TransactionTest, GuardUnlocksOnDatabaseException) {
    class MockTransaction : public Transaction {
    protected:
        void SaveToDataBase(Account& from, Account& to, int sum) override {
            throw std::runtime_error("DB error");
        }
    };

    MockTransaction mockTr;
    Account acc1(1, 1000);
    Account acc2(2, 500);
    
    ASSERT_FALSE(mockTr.Make(acc1, acc2, 100));
    ASSERT_NO_THROW(acc1.Lock()); 
    ASSERT_NO_THROW(acc2.Lock());
}
TEST(TransactionTest, DebitEdgeCaseBalanceEqualsSum) {
    TransactionTestFriend tr;
    Account acc(1, 100);
    acc.Lock();
    
    ASSERT_TRUE(tr.TestDebit(acc, 100));
    ASSERT_EQ(acc.GetBalance(), 0);
}



TEST(TransactionTest, DebitZeroSumThrows) {
    TransactionTestFriend tr;
    Account acc(1, 100);
    acc.Lock();
    ASSERT_THROW(tr.TestDebit(acc, 0), std::invalid_argument);
}


TEST(AccountTest, UnlockDoesNotThrowWhenUnlocked) {
    Account acc(1, 100);
    ASSERT_NO_THROW(acc.Unlock());
}
TEST(AccountTest, GetBalanceReturnsCorrectValue) {
    Account acc(1, 200);
    ASSERT_EQ(acc.GetBalance(), 200);
}
TEST(TransactionTest, BalanceExactlySumPlusFee) {
    Transaction tr;
    Account acc1(1, 101); 
    Account acc2(2, 0);
    ASSERT_TRUE(tr.Make(acc1, acc2, 100));
    ASSERT_EQ(acc1.GetBalance(), 0);
}
TEST(TransactionTest, GuardUnlockOrder) {
    class MockAccount : public Account {
    public:
        MockAccount(int id, int balance) : Account(id, balance) {}
        void Unlock() override {
            static bool toUnlocked = false;
            if (id() == 2) toUnlocked = true;
            if (id() == 1 && !toUnlocked) {
                ADD_FAILURE() << "from unlocked before to!";
            }
            Account::Unlock();
        }
    };

    MockAccount acc1(1, 1000);
    MockAccount acc2(2, 500);
    Transaction tr;
    tr.Make(acc1, acc2, 100);
}
TEST(TransactionTest, FeeGetter) {
    Transaction tr;
    ASSERT_EQ(tr.fee(), 1);
}
TEST(TransactionTest, SaveToDatabaseOutput) {
    testing::internal::CaptureStdout();
    Transaction tr;
    Account acc1(1, 1000);
    Account acc2(2, 500);
    tr.Make(acc1, acc2, 100);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "1 send to 2 $100\n");
}

TEST(TransactionTest, GuardUnlocksOnInsufficientBalance) {
    Transaction tr;
    Account acc1(1, 100); 
    Account acc2(2, 0);

    ASSERT_FALSE(tr.Make(acc1, acc2, 100));
    ASSERT_NO_THROW(acc1.Lock());
    ASSERT_NO_THROW(acc2.Lock());
    acc1.Unlock();
    acc2.Unlock();
}

TEST(TransactionTest, GuardUnlocksOnSuccess) {
    Transaction tr;
    Account acc1(1, 200);
    Account acc2(2, 0);

    ASSERT_TRUE(tr.Make(acc1, acc2, 100));
    ASSERT_NO_THROW(acc1.Lock());
    ASSERT_NO_THROW(acc2.Lock());
    acc1.Unlock();
    acc2.Unlock();
}


TEST(TransactionTest, CreditDebitInvalidSum) {
    TransactionTestFriend tr;
    Account acc(1, 100);
    acc.Lock();
    
    ASSERT_THROW(tr.TestCredit(acc, -50), std::invalid_argument);
    ASSERT_THROW(tr.TestDebit(acc, -50), std::invalid_argument);
    ASSERT_THROW(tr.TestCredit(acc, 0), std::invalid_argument);
    ASSERT_THROW(tr.TestDebit(acc, 0), std::invalid_argument);
}


TEST(AccountTest, ChangeBalanceThrowsWhenNotLocked) {
    Account acc(1, 100);
    ASSERT_THROW(acc.ChangeBalance(50), std::runtime_error);
}

TEST(AccountTest, ConstructorInitialization) {
    Account acc(42, 999);
    ASSERT_EQ(acc.id(), 42);
    ASSERT_EQ(acc.GetBalance(), 999);
}
TEST(TransactionTest, NegativeCredit) {
    TransactionTestFriend tr;
    Account acc(1, 100);
    acc.Lock();
    ASSERT_THROW(tr.TestCredit(acc, -50), std::invalid_argument);
}

TEST(TransactionTest, NegativeDebit) {
    TransactionTestFriend tr;
    Account acc(1, 100);
    acc.Lock();
    ASSERT_THROW(tr.TestDebit(acc, -50), std::invalid_argument);
}

TEST(TransactionTest, UnlockAfterException) {
    Account acc(1, 100);
    ASSERT_NO_THROW(acc.Unlock());
}
TEST(TransactionTest, SaveToDatabaseInSuccessCase) {
    class MockTransaction : public Transaction {
    public:
        bool saveCalled = false;
    protected:
        void SaveToDataBase(Account& from, Account& to, int sum) override {
            saveCalled = true;
            Transaction::SaveToDataBase(from, to, sum);
        }
    };

    MockTransaction mockTr;
    Account acc1(1, 1000);
    Account acc2(2, 500);
    
    ASSERT_TRUE(mockTr.Make(acc1, acc2, 100));
    ASSERT_TRUE(mockTr.saveCalled);
}
TEST(TransactionTest, GuardUnlocksOnInsufficientBalance) {
    Transaction tr;
    Account acc1(1, 100);
    Account acc2(2, 0);

    ASSERT_FALSE(tr.Make(acc1, acc2, 100));
    ASSERT_NO_THROW(acc1.Lock());
    ASSERT_NO_THROW(acc2.Lock());
    acc1.Unlock();
    acc2.Unlock();
}

TEST(TransactionTest, FeeChangeAffectsTransaction) {
    Transaction tr;
    tr.set_fee(5);
    Account acc1(1, 200);
    Account acc2(2, 0);

    ASSERT_TRUE(tr.Make(acc1, acc2, 100));
    ASSERT_EQ(acc1.GetBalance(), 95);
}
TEST(AccountTest, LockUnlockSequence) {
    Account acc(1, 100);
    acc.Lock();
    acc.Unlock();
    ASSERT_NO_THROW(acc.Lock());
}

TEST(TransactionTest, DebitExactBalanceAfterFee) {
    TransactionTestFriend tr;
    Account acc(1, 101);
    acc.Lock();
    ASSERT_TRUE(tr.TestDebit(acc, 101));
    ASSERT_EQ(acc.GetBalance(), 0);
}

TEST(TransactionTest, CreditAndDebitWithHighValues) {
    TransactionTestFriend tr;
    Account acc(1, 1000000);
    acc.Lock();
    tr.TestCredit(acc, 500000);
    ASSERT_EQ(acc.GetBalance(), 1500000);
    ASSERT_TRUE(tr.TestDebit(acc, 1000000));
    ASSERT_EQ(acc.GetBalance(), 500000);
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
