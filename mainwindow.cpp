#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->readers_count = 0;
    this->connect_serial_ports(false); //connect all serial ports with barcode readers without message box
    this->actual_buf = 0;

    this->today = QDate::currentDate();
    ui->date->setText(this->today.toString("dd-MM-yyyy"));

    this->timer.setSingleShot(false); //continous timer
    this->timer.setInterval(TIME_INTERVAL);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
    this->timer.start();

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

void MainWindow::connect_serial_ports(bool info)
{
    int readers_count_old = this->readers_count;
    this->readers_count = 0;

    //foreach from qt wiki
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.description() == "" && info.manufacturer() == "")
        {
            this->readers_count++;
        }

    }
    if (this->readers_count > readers_count_old && info) //false only when first time running
        QMessageBox::information(this, "Nowy czytnik", "Poprawnie skonfigurowany nowy czytnik");
    else if (this->readers_count < readers_count_old)
        QMessageBox::warning(this, "Odłączono czytnik", "Czytnik został odłączony");

    ui->readers_count->setText(QString::number(this->readers_count));
}

void MainWindow::get_key(int key)
{
    this->buffer[this->actual_buf] = key;

    this->actual_buf_inc();

    this->timer.start();
}

void MainWindow::get_line_feed()
{
    int temp[BUFFER_LEN];

    for (int i = 0; i < BUFFER_LEN; i++)
    {
        temp[i] = this->buffer[this->actual_buf]; //actual buffer is always one more than last digit place
        if (temp[i] < 0 || temp[i] > 9) //it isn't barcode
            return;
        this->buffer[this->actual_buf] = -1;

        this->actual_buf_inc();
    }
    this->actual_buf = 0;

    QString barcode;
    //I'm sure it is barcode, so I save it
    for (int i = 0; i < BUFFER_LEN; i++)
        barcode.append(QString::number(temp[i], 10));

    qDebug() << "Barcode: " << barcode;
    this->timer.stop();

    this->save_barcode(barcode);
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

void MainWindow::actual_buf_inc()
{
    if(this->actual_buf >= BUFFER_LEN - 1)
        this->actual_buf = 0;
    else
        this->actual_buf++;
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
