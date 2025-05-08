#pragma once
#include <mutex>

class Account {
public:
    Account(int id, int balance);
    virtual ~Account() = default;  // Добавляем виртуальный деструктор
    
    virtual int GetBalance() const;  // Делаем виртуальным
    virtual int id() const;
    virtual void ChangeBalance(int diff);  // Делаем виртуальным
    virtual void Lock();  // Делаем виртуальным
    virtual void Unlock();  // Делаем виртуальным
    
private:
    int id_;
    int balance_;
    std::mutex m_;
};
