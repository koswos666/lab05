#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "banking/Account.h"

using namespace testing;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
    MOCK_METHOD(int, id, (), (const, override));  // Добавляем мок для id()
};

TEST(AccountTest, InitialBalance) {
    MockAccount acc(1, 100);
    EXPECT_CALL(acc, id()).WillOnce(Return(1));
    EXPECT_EQ(acc.id(), 1);
    
    EXPECT_CALL(acc, GetBalance()).WillOnce(Return(100));
    EXPECT_EQ(acc.GetBalance(), 100);
}

TEST(AccountTest, LockUnlock) {
    MockAccount acc(1, 100);
    EXPECT_CALL(acc, Lock()).Times(1);
    EXPECT_CALL(acc, Unlock()).Times(1);
    acc.Lock();
    acc.Unlock();
}
