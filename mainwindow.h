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

    Ui::MainWindow *ui;
    QDir files_dir;
    QDate today;
};

#endif // MAINWINDOW_H
