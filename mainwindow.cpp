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
            out << this->files_dir.absolutePath();
        }
    }
    else
    {
        QFile file("ustawienia.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(this, "Bład otworzenia pliku", "Uruchom program ponownie");
            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection); //closing when fail
            return;
        }
        else
        {
            QByteArray line = file.readLine();
            this->files_dir.setPath(line);

            if(!files_dir.exists() || files_dir.path() == "")
            {
                QMessageBox::critical(this, "Błędny plik konfiguracyjny", "Uruchom program ponownie");
                QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection); //closing when fail
                return;
            }

            qDebug() << this->files_dir.path();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_testButton_clicked()
{
    QFile file;
    QDir::setCurrent(this->files_dir.path());
    file.setFileName(this->today.toString("yyyy-MM-dd") + ".txt"); //filename must be easy to sort
    //when fail
    if(!file.open(QIODevice::Append))
    {
        QMessageBox::critical(this, "Błąd zapisu do pliku", "Coś poszło nie tak, nie udało się zapisać do pliku."
                              "Zeskanuj towar ponownie");
        return;
    }
    else
    {
        QTextStream out(&file);
        out << "Kod kreskowy" << '\t' << QTime::currentTime().toString("hh:mm:ss") << '\r' << '\n';

        file.close();
    }
}

void MainWindow::on_actionO_autorze_triggered()
{
    QMessageBox::about(this, "O autorze", "Autor: <b>Marcin Twardak</b> <br>"
                                            "E-mail: <b>twardakm@gmail.com</b>");
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}
