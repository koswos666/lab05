#include "banking/Transaction.h"
#include "banking/Account.h"

Transaction::Transaction() : locked_(false) {}

bool Transaction::Make(Account& from, Account& to, int amount) {
    if (from.id() == to.id()) return false;
    if (amount < 0) return false;
    
    // Сначала проверяем баланс без блокировки
    from.Lock();
    int balance = from.GetBalance();
    from.Unlock();
    
    if (balance < amount) {
        return false;
    }
    
    // Только если средств достаточно, выполняем транзакцию с блокировкой
    from.Lock();
    to.Lock();
    
    from.ChangeBalance(-amount);
    to.ChangeBalance(amount);
    
    from.Unlock();
    to.Unlock();
    return true;
}
