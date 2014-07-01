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
#include <windows.h>

#define BUFFER_LEN 13 //amount of digits in barcode

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend LRESULT CALLBACK MyLowLevelKeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

private slots:
    void on_testButton_clicked();

    void on_actionO_autorze_triggered();

    void on_actionAbout_Qt_triggered();

private:
    void get_key(int key);
    void get_line_feed(); //if line feed is pressed
    void actual_buf_inc();

    Ui::MainWindow *ui;
    QDir files_dir;
    QDate today;
    int buffer[BUFFER_LEN];
    int actual_buf;
};

#endif // MAINWINDOW_H
