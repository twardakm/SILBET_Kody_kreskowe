#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->today = QDate::currentDate();
    ui->date->setText(this->today.toString("dd-MM-yyyy"));

    //finding a path to save logs

    if(!QFile::exists("ustawienia.txt"))
    {
        QMessageBox::information(this, "Pierwsze uruchomienie programu", "Wybierz folder do zapisu danych");
        QString dir = QFileDialog::getExistingDirectory(this, tr("Wybierz folder"),
                                                         QDir::homePath(),
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);

        if (dir == "")
        {
            QMessageBox::critical(this, "Nie wybrano katalogu", "Nie wybrano katalogu. Uruchom program ponownie");
            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection); //closing when cancel
            return;
        }
        else
            this->files_dir.setPath(dir); //setting up the directory

        QFile file("ustawienia.txt");
        if(!file.open(QIODevice::ReadWrite))
        {
            QMessageBox::critical(this, "Bład utworzenia pliku", "Uruchom program ponownie");
            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection); //closing when fail
            return;
        }
        else
        {
            QTextStream out(&file);
            out << this->files_dir.absolutePath() << ";";
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}