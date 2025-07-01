#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("个人财务管理系统");
    w.resize(1000, 700);
    w.show();
    return a.exec();
}
