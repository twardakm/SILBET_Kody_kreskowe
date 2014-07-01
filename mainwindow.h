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

#define BUFFER_LEN 13 //amount of digits in barcode
#define TIME_INTERVAL 5000 //how often will be checked new devices

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
    void get_key(int key);
    void get_line_feed(); //if line feed is pressed
    void actual_buf_inc();

    void connect_serial_ports(bool info = true);

    void save_barcode(QString barcode);

    Ui::MainWindow *ui;
    QDir files_dir;
    QDate today;
    QTimer timer;
    int buffer[BUFFER_LEN];
    int actual_buf;
    int readers_count;
};

#endif // MAINWINDOW_H
