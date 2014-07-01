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
#include <windows.h>

#define BUFFER_LEN 13 //amount of digits in barcode
#define TIME_INTERVAL 100 //time needed to clear buffer (user can't write so fast)

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

    void save_barcode(QString barcode);

    Ui::MainWindow *ui;
    QDir files_dir;
    QDate today;
    QTimer timer;
    int buffer[BUFFER_LEN];
    int actual_buf;
};

#endif // MAINWINDOW_H
