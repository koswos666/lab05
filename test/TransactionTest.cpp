#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "banking/Account.h"
#include "banking/Transaction.h"

using namespace testing;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
    MOCK_METHOD(int, id, (), (const, override));
};

// ... (остальные тесты остаются без изменений) ...

TEST(TransactionTest, InsufficientFunds) {
    MockAccount from(1, 100);
    MockAccount to(2, 500);
    Transaction transaction;

    // Устанавливаем ожидания для from
    EXPECT_CALL(from, id()).WillRepeatedly(Return(1));
    EXPECT_CALL(from, Lock()).Times(1);  // Теперь ожидаем 1 вызов Lock
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(100));
    EXPECT_CALL(from, Unlock()).Times(1);  // Теперь ожидаем 1 вызов Unlock
    EXPECT_CALL(from, ChangeBalance(_)).Times(0);

    // Устанавливаем ожидания для to
    EXPECT_CALL(to, id()).WillRepeatedly(Return(2));
    EXPECT_CALL(to, Lock()).Times(0);
    EXPECT_CALL(to, ChangeBalance(_)).Times(0);
    EXPECT_CALL(to, Unlock()).Times(0);

    EXPECT_FALSE(transaction.Make(from, to, 300));
}
