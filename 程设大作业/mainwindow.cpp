#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化模型
    transactionModel = new QStandardItemModel(0, 5, this);
    transactionModel->setHorizontalHeaderLabels({"日期", "类型", "类别", "金额", "描述"});
    ui->transactionTableView->setModel(transactionModel);

    accountModel = new QStandardItemModel(0, 3, this); // 修改为3列
    accountModel->setHorizontalHeaderLabels({"账户名称", "账户类型", "余额"}); // 添加“账户类型”
    ui->accountListView->setModel(accountModel);

    setupUI();
    setupConnections();
    loadInitialData();
    updateUI();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupUI() {
    // 设置日期范围
    ui->dateFromEdit->setDate(QDate::currentDate().addMonths(-1));
    ui->dateToEdit->setDate(QDate::currentDate());

    // 初始化预算表格
    QStringList categories = {"食品", "交通", "娱乐", "住房", "医疗"};
    ui->budgetCategoryCombo->addItems(categories);

    // 设置账户类型选项
    QStringList accountTypes = {"现金", "银行卡", "信用卡"};
    ui->accountTypeCombo->addItems(accountTypes); // 新增

    // 设置表格样式
    ui->transactionTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::setupConnections() {
    // 记录管理
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::addTransaction);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::deleteTransaction);

    // 账户管理
    connect(ui->addAccountButton, &QPushButton::clicked, this, &MainWindow::addAccount);
    connect(ui->deleteAccountButton, &QPushButton::clicked, this, &MainWindow::deleteAccount);
    connect(ui->accountListView, &QListView::clicked, this, &MainWindow::switchAccount);

    // 预算管理
    connect(ui->setBudgetButton, &QPushButton::clicked, this, &MainWindow::updateBudget);

    // 数据分析与报表
    connect(ui->exportButton, &QPushButton::clicked, this, &MainWindow::exportReport);
    connect(ui->filterButton, &QPushButton::clicked, this, &MainWindow::applyDateFilter);

    // 数据变化时更新UI
    connect(ui->accountListView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateUI);
}

void MainWindow::loadInitialData() {
    // 添加示例账户
    accounts.append(Account("现金账户", "现金", 0.0));
    accounts.append(Account("银行账户", "银行卡", 7914.50));

    // 添加示例交易
    transactions.append(Transaction(QDate::currentDate(), "支出", "食品", 85.50, "超市购物"));
    transactions.append(Transaction(QDate::currentDate().addDays(-3), "收入", "工资", 8000.0, "月工资"));

    // 初始化预算
    budgets["食品"] = 1000.0;
    budgets["交通"] = 500.0;
    budgets["娱乐"] = 300.0;

    // 更新账户列表
    for (const Account &account : accounts) {
        QList<QStandardItem*> items;
        items << new QStandardItem(account.getName());
        items << new QStandardItem(account.getType()); // 显示账户类型
        items << new QStandardItem(QString::number(account.getBalance(), 'f', 2));
        accountModel->appendRow(items);
    }
}

// 添加记录
void MainWindow::addTransaction() {
    double amount = ui->amountSpin->value();
    QDate date = ui->dateEdit->date();
    if (amount <= 0) {
        QMessageBox::warning(this, "错误", "金额必须为正数！");
        return;
    }
    if (!date.isValid()) {
        QMessageBox::warning(this, "错误", "日期无效！");
        return;
    }
    Transaction trans(
        ui->dateEdit->date(),
        ui->typeCombo->currentText(),
        ui->categoryCombo->currentText(),
        ui->amountSpin->value(),
        ui->descriptionEdit->text()
        );

    transactions.append(trans);
    accounts[currentAccountIndex].updateBalance(trans);

    // 更新表格
    QList<QStandardItem*> items;
    items << new QStandardItem(trans.date.toString("yyyy-MM-dd"));
    items << new QStandardItem(trans.type);
    items << new QStandardItem(trans.category);
    items << new QStandardItem(QString::number(trans.amount, 'f', 2));
    items << new QStandardItem(trans.description);
    transactionModel->appendRow(items);

    updateFinancialOverview();
    updateUI();
}

// 删除记录
void MainWindow::deleteTransaction() {
    QModelIndex index = ui->transactionTableView->currentIndex();
    if (index.isValid()) {
        int row = index.row();
        Transaction trans = transactions[row];
        accounts[currentAccountIndex].reverseTransaction(trans);
        transactions.removeAt(row);
        transactionModel->removeRow(row);
        updateFinancialOverview();
        updateUI();
    }
}

// 添加账户
void MainWindow::addAccount() {
    QString name = ui->accountNameEdit->text();
    QString type = ui->accountTypeCombo->currentText(); // 获取账户类型
    if (name.isEmpty() || type.isEmpty()) {
        QMessageBox::warning(this, "错误", "账户名称和类型不能为空！");
        return;
    }
    Account newAccount(name, type, 0.0);
    accounts.append(newAccount);

    QList<QStandardItem*> items;
    items << new QStandardItem(newAccount.getName());
    items << new QStandardItem(newAccount.getType()); // 显示账户类型
    items << new QStandardItem("0.00");
    accountModel->appendRow(items);

    ui->accountNameEdit->clear();
    updateUI();
}

// 删除账户
void MainWindow::deleteAccount() {
    QModelIndex index = ui->accountListView->currentIndex();
    if (index.isValid() && accounts.size() > 1) {
        int row = index.row();
        accounts.removeAt(row);
        accountModel->removeRow(row);
        if (currentAccountIndex >= accounts.size()) {
            currentAccountIndex = accounts.size() - 1;
        }
        updateUI();
    }
}

// 切换账户
void MainWindow::switchAccount(const QModelIndex &index) {
    if (index.isValid()) {
        currentAccountIndex = index.row();
        transactionModel->removeRows(0, transactionModel->rowCount());

        // 显示当前账户的交易
        for (const Transaction &trans : transactions) {
            QList<QStandardItem*> items;
            items << new QStandardItem(trans.date.toString("yyyy-MM-dd"));
            items << new QStandardItem(trans.type);
            items << new QStandardItem(trans.category);
            items << new QStandardItem(QString::number(trans.amount, 'f', 2));
            items << new QStandardItem(trans.description);
            transactionModel->appendRow(items);
        }

        updateFinancialOverview();
    }
}

// 更新预算
void MainWindow::updateBudget() {
    QString category = ui->budgetCategoryCombo->currentText();
    double amount = ui->budgetAmountSpin->value();
    budgets[category] = amount;

    QMessageBox::information(this, "预算更新",
                             QString("'%1' 的预算已设置为: %2").arg(category).arg(amount));
}

// 财务概览
void MainWindow::updateFinancialOverview() {
    double income = 0, expense = 0;
    for (const Transaction &t : transactions) {
        if (t.type == "收入") income += t.amount;
        else expense += t.amount;
    }

    double balance = accounts[currentAccountIndex].getBalance();

    ui->incomeLabel->setText(QString("总收入: %1").arg(income));
    ui->expenseLabel->setText(QString("总支出: %1").arg(expense));
    ui->balanceLabel->setText(QString("当前余额: %1").arg(balance));

    // 预算使用情况
    QString budgetText = "预算情况:\n";
    QMapIterator<QString, double> i(budgets);
    while (i.hasNext()) {
        i.next();
        budgetText += QString("%1: %2\n").arg(i.key()).arg(i.value());
    }
    ui->budgetText->setText(budgetText);
}

// 导出报表
void MainWindow::exportReport() {
    QString fileName = QFileDialog::getSaveFileName(this, "导出报表", "", "CSV文件 (*.csv)");
    if (!fileName.isEmpty()) {
        if (dataHandler.exportToCSV(transactions, fileName)) {
            QMessageBox::information(this, "导出成功", "数据已成功导出到CSV文件");
        } else {
            QMessageBox::warning(this, "导出失败", "无法导出数据到文件");
        }
    }
}

// 时间过滤
void MainWindow::applyDateFilter() {
    QDate from = ui->dateFromEdit->date();
    QDate to = ui->dateToEdit->date();

    transactionModel->removeRows(0, transactionModel->rowCount());

    for (const Transaction &t : transactions) {
        if (t.date >= from && t.date <= to) {
            QList<QStandardItem*> items;
            items << new QStandardItem(t.date.toString("yyyy-MM-dd"));
            items << new QStandardItem(t.type);
            items << new QStandardItem(t.category);
            items << new QStandardItem(QString::number(t.amount, 'f', 2));
            items << new QStandardItem(t.description);
            transactionModel->appendRow(items);
        }
    }
}

// 更新UI
void MainWindow::updateUI() {
    updateFinancialOverview();
    ui->accountLabel->setText("当前账户: " + accounts[currentAccountIndex].getName());
}
