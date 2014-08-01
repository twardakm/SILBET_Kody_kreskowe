#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //refresh drivers
    system ("\"C:\\Program Files\\Symbol\\Symbol Scanners\\Symbol Update driver.exe\"");
    ui->setupUi(this);
    //connect all readers with slot
    for (int i = 0; i < MAX_DEVICES; i++)
        connect(&this->devices[i], SIGNAL(readyRead()), this, SLOT(newData()));

    this->readers_count = 0;
    this->connect_serial_ports(false); //connect all serial ports with barcode readers without message box

    this->today = QDate::currentDate();
    ui->date->setText(this->today.toString("dd-MM-yyyy"));

    this->timer.setSingleShot(false); //continous timer
    this->timer.setInterval(TIME_INTERVAL);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
    this->timer.start();

    this->exe_dir = QDir::currentPath();

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
    //closing connections
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (this->devices[i].isOpen())
            this->devices[i].close();

    }
    delete ui;
}

void MainWindow::connect_serial_ports(bool info)
{
    int readers_count_old = this->readers_count;
    this->readers_count = 0;

    //foreach from qt wiki
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (READER_INFO)
        {
            this->readers_count++;
            if (this->readers_count > MAX_DEVICES) //too many devices
            {
                QMessageBox::critical(this, "Zbyt dużo urządzeń", "Zbyt dużo urządzeń. Odłącz urządzenie i uruchom program ponownie");
                QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
                return;
            }
        }

    }
    if (!info || error) //only first time or if error!
    {
        if(!this->open_connections())
            QMessageBox::warning(this, "Błąd", "Nie można było połączyć się z czytnikiem. <br>"
                                 "Jeśli komunikat będzie się powtarzał uruchom ponownie aplikację");
    }
    else if (this->readers_count > readers_count_old && info) //false only when first time running
    {
        if(!this->open_connections())
            QMessageBox::warning(this, "Błąd", "Nie można było połączyć się z czytnikiem. <br>"
                                 "Jeśli komunikat będzie się powtarzał uruchom ponownie aplikację");
        else
            QMessageBox::information(this, "Nowy czytnik", "Poprawnie skonfigurowano nowy czytnik");
    }
    else if (this->readers_count < readers_count_old)
    {
        //close old connections
        for (int i = 0; i < MAX_DEVICES; i++)
            if (this->devices[i].isOpen())
                this->devices[i].close();

        if(!this->open_connections())
            QMessageBox::warning(this, "Błąd", "Nie można było połączyć się z czytnikiem. <br>"
                                 "Jeśli komunikat będzie się powtarzał uruchom ponownie aplikację");
        else
            QMessageBox::warning(this, "Odłączono czytnik", "Czytnik został odłączony");
    }

    ui->readers_count->setText(QString::number(this->readers_count));
}

bool MainWindow::open_connections()
{
    int i = 0;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (READER_INFO)
        {
            if (this->devices[i].isOpen())
                this->devices[i].close();
            //open_connection
            this->devices[i].setPort(info);
            if (!this->devices[i].setBaudRate(QSerialPort::Baud9600) ||
                !this->devices[i].setFlowControl(QSerialPort::NoFlowControl) ||
                !this->devices[i].setParity(QSerialPort::NoParity)
            )
                return false; //something went wront

            else if ( !this->devices[i].open(QIODevice::ReadOnly))
                return false;

            i++;
        }

    }
    return true;
}

void MainWindow::newData()
{
    char *temp = new char[MAX_LENGHT];
    QString str;
    QStringList list;

    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (this->devices[i].canReadLine())
        {
            this->devices[i].read(temp, MAX_LENGHT);
            str = temp;
            list = str.split('\r');
            if (this->last_barcode != list.at(0)) // to avoid double barcodes
            {
                this->save_barcode(list.at(0));
                this->last_barcode = list.at(0);
            }
        }
    }
}

void MainWindow::save_barcode(QString barcode)
{
    QFile file;
    QDir::setCurrent(this->files_dir.path());
    file.setFileName(this->today.toString("yyyy-MM-dd") + ".txt"); //filename must be easy to sort
    //when fail
    if(!file.open(QIODevice::ReadOnly) && file.exists())
    {
        QMessageBox::critical(this, "Błąd zapisu do pliku", "Coś poszło nie tak, nie udało się zapisać do pliku."
                              "Zeskanuj towar ponownie");
        return;
    }
    else
    {
        //first - reading number of lines
        int number_of_lines = 1;
        QTextStream in(&file);
        while( !in.atEnd())
        {
            in.readLine();
            number_of_lines++;
        }
        file.close(); //close ReadOnly, open Append

        if(!file.open(QIODevice::Append | QIODevice::Text))
        {
            QMessageBox::critical(this, "Błąd zapisu do pliku", "Coś poszło nie tak, nie udało się zapisać do pliku."
                                  "Zeskanuj towar ponownie");
            return;
        }
        else
        {
            QTextStream out(&file);
            out << number_of_lines << '\t' << barcode <<
                   '\t' << QTime::currentTime().toString("hh:mm:ss") << '\r' << '\n';

            file.close();
        }
    }
}

void MainWindow::timer_timeout()
{
    this->connect_serial_ports();
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

void MainWindow::on_actionFolder_zapisu_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Wybierz folder"),
                                                     QDir::homePath(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    if (dir != "")
    {
        QDir::setCurrent(this->exe_dir.absolutePath());
        QFile file("ustawienia.txt");
        if(!file.open(QIODevice::WriteOnly))
            QMessageBox::critical(this, "Błąd zapisu", "Błąd zapisu ustawień");
        else
        {
            this->files_dir = dir;
            QTextStream out(&file);
            out << this->files_dir.absolutePath();
        }
    }
}
