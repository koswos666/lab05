#pragma once
#include "Account.h"

class Transaction {
public:
    Transaction();
    bool Make(Account& from, Account& to, int amount);
    
private:
    bool locked_;
};
