#include <gtest/gtest.h>
#include "Account.h"

TEST(AccountTest, InitialBalanceIsCorrect) {
    Account acc(1, 1000);
    ASSERT_EQ(acc.GetBalance(), 1000);
}

TEST(AccountTest, BalanceChangeRequiresLock) {
    Account acc(1, 1000);
    ASSERT_THROW(acc.ChangeBalance(100), std::runtime_error);
    acc.Lock();
    ASSERT_NO_THROW(acc.ChangeBalance(100));
    ASSERT_EQ(acc.GetBalance(), 1100);
}

TEST(AccountTest, DoubleLockThrowsException) {
    Account acc(1, 100);
    acc.Lock();
    ASSERT_THROW(acc.Lock(), std::runtime_error);
}

TEST(AccountTest, UnlockUnlockedAccountSucceeds) {
    Account acc(1, 100);
    ASSERT_NO_THROW(acc.Unlock());
}
TEST(AccountTest, GetIdReturnsCorrectValue) {
    Account acc(42, 100);
    ASSERT_EQ(acc.id(), 42);
}

TEST(AccountTest, UnlockedBalanceChangeThrows) {
    Account acc(1, 100);
    ASSERT_THROW(acc.ChangeBalance(50), std::runtime_error);
}
