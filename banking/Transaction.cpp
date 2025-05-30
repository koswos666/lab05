#include "Transaction.h"
#include "Account.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

namespace {
    struct Guard {
        Guard(Account& account) : account_(&account) { account_->Lock(); }
        ~Guard() { account_->Unlock(); }
    private:
        Account* account_;
    };
} 

Transaction::Transaction() : fee_(1) {}
Transaction::~Transaction() = default;

bool Transaction::Make(Account& from, Account& to, int sum) {
    if (from.id() == to.id()) throw std::logic_error("invalid action");
    if (sum < 0) throw std::invalid_argument("sum can't be negative");
    if (fee_ * 2 > sum) return false;
    if (sum < 100) throw std::logic_error("too small");

    { 
        Guard guard_from(from);
        Guard guard_to(to);

        if (!Debit(from, sum + fee_)) {
            return false;
        }
        Credit(to, sum);
    } 

    SaveToDataBase(from, to, sum);
    return true;
}

void Transaction::Credit(Account& account, int sum) {
    account.ChangeBalance(sum);
}

bool Transaction::Debit(Account& account, int sum) {
    if (account.GetBalance() >= sum) {
        account.ChangeBalance(-sum);
        return true;
    }
    return false;
}
void Transaction::SaveToDataBase(Account& from, Account& to, int sum) {
    
    fprintf(stdout, "%d send to %d $%d\n", from.id(), to.id(), sum);
    fprintf(stdout, "Balance %d is %d\n", from.id(), from.GetBalance());
    fprintf(stdout, "Balance %d is %d\n", to.id(), to.GetBalance());
    fflush(stdout); 
}
