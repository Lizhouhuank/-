#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include "transaction.h"

class Account {
public:
    Account(QString name, QString type, double balance)
        : name(name), type(type), balance(balance) {}

    QString getName() const { return name; }
    QString getType() const { return type; } // 新增 getter
    double getBalance() const { return balance; }

    void updateBalance(const Transaction &t) {
        if (t.type == "收入") {
            balance += t.amount;
        } else {
            balance -= t.amount;
        }
    }

    void reverseTransaction(const Transaction &t) {
        if (t.type == "收入") {
            balance -= t.amount;
        } else {
            balance += t.amount;
        }
    }

private:
    QString name;
    QString type; // 新增 type 成员
    double balance;
};

#endif // ACCOUNT_H
