#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QDate>
#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define TIME_INTERVAL 5000 //how often will be checked new devices
#define MAX_DEVICES 10
#define READER_INFO info.description() == "" && info.manufacturer() == ""

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

private slots:
    void on_actionO_autorze_triggered();

    void on_actionAbout_Qt_triggered();

    void timer_timeout();

private:
    void connect_serial_ports(bool info = true);
    bool open_connections();

    void save_barcode(QString barcode);

    Ui::MainWindow *ui;
    QDir files_dir;
    QDate today;
    QTimer timer;
    int readers_count;
    bool error; //if error is true open connections is making again

    QSerialPort devices[MAX_DEVICES];
};

#endif // MAINWINDOW_H
