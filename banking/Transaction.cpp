#include "Transaction.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "Account.h"

namespace {
// RAII
struct Guard {
  Guard(Account& account) : account_(&account) { account_->Lock(); }

  ~Guard() { account_->Unlock(); }

 private:
  Account* account_;
};
}  // namespace

Transaction::Transaction() : fee_(1) {}

Transaction::~Transaction() {}

bool Transaction::Make(Account& from, Account& to, int sum) {
  if (from.id() == to.id()) throw std::logic_error("invalid action");
  if (sum < 0) throw std::invalid_argument("sum can't be negative");
  if (sum < 100) throw std::logic_error("too small");

  Guard guard_from(from);
  Guard guard_to(to);

  // Проверяем достаточно ли средств с учетом комиссии
  if (from.GetBalance() < sum + fee_) {
    return false;
  }

  // Выполняем транзакцию
  Credit(to, sum);
  Debit(from, sum + fee_);

  SaveToDataBase(from, to, sum);
  return true;
}

void Transaction::Credit(Account& accout, int sum) {
  assert(sum > 0);
  accout.ChangeBalance(sum);
}

bool Transaction::Debit(Account& accout, int sum) {
  assert(sum > 0);
  if (accout.GetBalance() > sum) {
    accout.ChangeBalance(-sum);
    return true;
  }
  return false;
}

void Transaction::SaveToDataBase(Account& from, Account& to, int sum) {
    std::cout << from.id() << " send to " << to.id() << " $" << sum << std::endl;
    std::cout << "Balance " << from.id() << " is " << from.GetBalance() << std::endl;
    std::cout << "Balance " << to.id() << " is " << to.GetBalance() << std::endl;
}
