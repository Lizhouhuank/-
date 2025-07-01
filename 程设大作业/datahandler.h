#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QList>
#include <QFile>
#include <QTextStream>
#include "transaction.h"

class DataHandler {
public:
    bool exportToCSV(const QList<Transaction> &transactions, const QString &fileName) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }

        QTextStream out(&file);
        out << "日期,类型,类别,金额,描述\n";

        for (const Transaction &t : transactions) {
            out << t.date.toString("yyyy-MM-dd") << ","
                << t.type << ","
                << t.category << ","
                << t.amount << ","
                << t.description << "\n";
        }

        file.close();
        return true;
    }
};

#endif // DATAHANDLER_H
