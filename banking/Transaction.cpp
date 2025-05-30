#include "Transaction.h"
#include "Account.h"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace {
    struct Guard {
        Guard(Account& account) : account_(account) { account_.Lock(); }
        ~Guard() { account_.Unlock(); }
    private:
        Account& account_;
    };
}

Transaction::Transaction() : fee_(1) {}

Transaction::~Transaction() {}

bool Transaction::Make(Account& from, Account& to, int sum) {
    if (from.id() == to.id()) throw std::logic_error("invalid action");
    if (sum < 0) throw std::invalid_argument("sum can't be negative");
    if (sum < 100) throw std::logic_error("too small");

    Guard guard_from(from);
    Guard guard_to(to);

    if (from.GetBalance() < sum + fee_) {
        return false;
    }

    try {
        SaveToDataBase(from, to, sum);
    } catch (...) {
        return false;
    }

    Credit(to, sum);
    Debit(from, sum + fee_);
    return true;
}

void Transaction::Credit(Account& account, int sum) {
    if (sum <= 0) throw std::invalid_argument("Credit sum must be positive");
    account.ChangeBalance(sum);
}

bool Transaction::Debit(Account& account, int sum) {
    if (sum <= 0) throw std::invalid_argument("Debit sum must be positive");
    if (account.GetBalance() < sum) {
        return false;
    }
    account.ChangeBalance(-sum);
    return true;
}

void Transaction::SaveToDataBase(Account& from, Account& to, int sum) {
    std::cout << from.id() << " send to " << to.id() << " $" << sum << "\n";
}
