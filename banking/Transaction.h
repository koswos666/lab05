#pragma once

class Account;

class Transaction {
public:
    friend class TransactionTestFriend;  // Дружественный класс
    
    Transaction();
    virtual ~Transaction();

    bool Make(Account& from, Account& to, int sum);
    int fee() const { return fee_; }
    void set_fee(int fee) { fee_ = fee; }

protected:
    virtual void SaveToDataBase(Account& from, Account& to, int sum);

private:
    void Credit(Account& account, int sum);
    bool Debit(Account& account, int sum);
    int fee_ = 1;
};
