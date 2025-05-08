#include "banking/Account.h"

Account::Account(int id, int balance) : id_(id), balance_(balance) {}

int Account::GetBalance() const { return balance_; }
int Account::id() const { return id_; }

void Account::ChangeBalance(int diff) { balance_ += diff; }

void Account::Lock() { m_.lock(); }
void Account::Unlock() { m_.unlock(); }
