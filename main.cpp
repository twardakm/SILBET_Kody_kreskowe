#include "mainwindow.h"
#include <QApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

QT_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed (true);
    static MainWindow w;
    w.show();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
           qDebug() << "Name        : " << info.portName();
           qDebug() << "Description : " << info.description();
           qDebug() << "Manufacturer: " << info.manufacturer();
    }

    return a.exec();
}
