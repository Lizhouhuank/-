#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QDate>
#include <QString>

class Transaction {
public:
    Transaction() = default;
    Transaction(QDate date, QString type, QString category, double amount, QString description)
        : date(date), type(type), category(category), amount(amount), description(description) {}

    QDate date;
    QString type;
    QString category;
    double amount;
    QString description;
};

#endif // TRANSACTION_H
