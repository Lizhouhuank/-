#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMessageBox>
#include "transaction.h"
#include "account.h"
#include "datahandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addAccount();
    void deleteAccount();
    void switchAccount(const QModelIndex &index);
    void addTransaction();
    void deleteTransaction();
    void updateBudget();
    void updateFinancialOverview();
    void exportReport();
    void applyDateFilter();
    void updateUI();

private:
    Ui::MainWindow *ui;
    DataHandler dataHandler;
    QList<Transaction> transactions;
    QList<Account> accounts;
    QMap<QString, double> budgets;
    int currentAccountIndex = 0;

    QStandardItemModel *transactionModel;
    QStandardItemModel *accountModel;

    void setupUI();
    void setupConnections();
    void loadInitialData();
};

#endif // MAINWINDOW_H
