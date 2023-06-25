#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtCore"
#include "QtGui"
#include "QMessageBox"
#include <QSerialPort>
#include <QtDebug>
#include <QString>
#include <QPixmap>
#include <QPalette>
#include <qwt_dial.h>
#include <qwt_dial_needle.h>
#include <qwt_scale_engine.h>

QSerialPort serial;
QStandardItemModel* console_model = new QStandardItemModel();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Set the background
    QPixmap bkgnd(":/rec/Images/blackbackground.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);

    // Load the image from a file
    QPixmap pixmap(":/rec/Images/up.png");

    // Set the size of the icon
    QSize iconSize(50, 50);

    // Load the image for the DistanceDecrement button
    QPixmap distanceDecrementPixmap(":/rec/Images/down.png");

    // Load the image for the SpeedIncrement button
    QPixmap speedIncrementPixmap(":/rec/Images/up.png");
    ui->SpeedIncrement->setIcon(speedIncrementPixmap.scaled(iconSize));
    ui->SpeedIncrement->setFixedSize(iconSize);

    // Load the image for the SpeedDecrement button
    QPixmap speedDecrementPixmap(":/rec/Images/down.png");
    ui->SpeedDecrement->setIcon(speedDecrementPixmap.scaled(iconSize));
    ui->SpeedDecrement->setFixedSize(iconSize);

    // Set the Normal mode radio button as checked
    ui->NomralMode->setChecked(true);

    // Send the Normal mode value over UART
    QByteArray uart_data = "0";
    UART_SendData(uart_data);

    // Output a message to the console
    qDebug() << "Sent data: " << uart_data;

    // Create a QFrame widget with a yellow background color
    QFrame* rectangle = new QFrame(this);
    rectangle->setStyleSheet("background-color: yellow;");
    rectangle->setFixedSize(50, 50);
    rectangle->move(10, 10);

    // Hide the rectangle initially
    rectangle->hide();

    // Create a QFrame widget with a yellow background color
    QFrame* rectangle1 = new QFrame(this);
    rectangle1->setStyleSheet("background-color: orange;");
    rectangle1->setFixedSize(50, 50);
    rectangle1->move(10, 10);

    // Hide the rectangle initially
    rectangle1->hide();

    // Create a QFrame widget with a yellow background color
    QFrame* rectangle2 = new QFrame(this);
    rectangle2->setStyleSheet("background-color: red;");
    rectangle2->setFixedSize(50, 50);
    rectangle2->move(10, 10);

    // Hide the rectangle initially
    rectangle2->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::UART_Init(const QString& device_name, int baud_rate)
{
    // Configure the serial port settings
    serial.setPortName(device_name);
    serial.setBaudRate(baud_rate);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    // Open the serial port
    if (serial.open(QIODevice::ReadWrite))
    {
        qDebug() << "Serial port opened successfully";
    }
    else
    {
        qDebug() << "Failed to open serial port";
    }

    // Connect the readyRead signal to the updateDialValue slot
    QObject::connect(&serial, &QSerialPort::readyRead, this, &MainWindow::updateDialValue);
}

void MainWindow::UART_SendData(const QByteArray& data)
{
    if (serial.isOpen())
    {
        // Send data over the serial port
        if (serial.write(data) != -1)
        {
            qDebug() << "Data sent: " << data;
            serial.clear();
            serial.close();
        }
        else
        {
            qDebug() << "Failed to send data";
        }
    }
    else
    {
        qDebug() << "Serial port is not open";
    }
}

QByteArray MainWindow::UART_ReceiveData(int length)
{
    // Open the serial port
    if (serial.open(QIODevice::ReadWrite))
    {
        // Wait for data to be available for reading
        while (serial.bytesAvailable() < length)
        {
            QCoreApplication::processEvents();
        }

        // Read the data from the serial port
        QByteArray data = serial.read(length);
        qDebug() << "Data received: " << data;

        // Close the serial port
        serial.clear();
        serial.close();
        return data;
    }
    else
    {
        qDebug() << "Failed to open serial port";
        return QByteArray();
    }
}

void MainWindow::on_SetSpeed_clicked()
{
    // Get the distance value from the SpeedEdit line edit
    QString distance_str = ui->SpeedEdit->text();
    bool ok;
    int distance = distance_str.toInt(&ok);

    // Check if the distance value was entered correctly
    if (!ok)
    {
        QMessageBox::warning(this, "Error", "Invalid distance value entered");
        return;
    }

    // Convert the distance value to a QByteArray
    QByteArray data;
    data.append(static_cast<char>(distance));

    // Send the data over UART
    UART_SendData(data);
}

void MainWindow::on_SpeedIncrement_clicked()
{
    // Get the current speed value from the SpeedEdit line edit, or set it to 10 if the line edit is empty
    QString speed_str = ui->SpeedEdit->text();
    bool ok;
    int speed = speed_str.isEmpty() ? 10 : speed_str.toInt(&ok);

    // Increment the speed value by 5
    speed += 5;

    // Limit the speed value to between 10 and 210
    if (speed > 210) {
        speed = 210;
    }

    // Update the SpeedEdit line edit with the new speed value
    ui->SpeedEdit->setText(QString::number(speed));

    // Send the data over UART
    QByteArray data = "7";
    if (serial.isOpen())
    {
        // Send data over the serial port
        if (serial.write(data) != -1)
        {
            qDebug() << "Data sent: " << data;
            serial.clear();
        }
        else
        {
            qDebug() << "Failed to send data";
        }
    }
    else
    {
        qDebug() << "Serial port is not open";
    }
    qDebug() << "Sent data: " << data;
}


void MainWindow::on_SpeedDecrement_clicked()
{
    // Get the current speed value from the SpeedEdit line edit
    QString speed_str = ui->SpeedEdit->text();
    bool ok;
    int speed = speed_str.toInt(&ok);

    // Decrement the speed value by 5
    speed -= 5;

    // Limit the speed value to between 10 and 210
    if (speed < 10) {
        speed = 10;
    }

    // Update the SpeedEdit line edit with the new speed value
    ui->SpeedEdit->setText(QString::number(speed));

    // Send the data over UART
    QByteArray data = "8";    if (serial.isOpen())
    {
        // Send data over the serial port
        if (serial.write(data) != -1)
        {
            qDebug() << "Data sent: " << data;
            serial.clear();
        }
        else
        {
            qDebug() << "Failed to send data";
        }
    }
    else
    {
        qDebug() << "Serial port is not open";
    }
    qDebug() << "Sent data: " << data;
}


void MainWindow::on_NomralMode_clicked()
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()
    // Send '1'
    QByteArray uart_data = "1";
    UART_SendData(uart_data);

    // Output a message to the console
    qDebug() << "Sent data: " << uart_data;

    // Clear the SpeedEdit line edit
    ui->SpeedEdit->clear();
}


void MainWindow::on_ACCMode_clicked()
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()
        // Set the speed value to 60
        ui->SpeedEdit->setText("55");

        // Send the data over UART
        QByteArray uart_data = "2";
        UART_SendData(uart_data);

        // Output a message to the console
        qDebug() << "Sent data: " << uart_data;
}


void MainWindow::on_DistanceShort_clicked()
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()
    // Send the distance value for short distance mode over UART
    QByteArray uart_data = "4";
    UART_SendData(uart_data);

    // Output a message to the console
    qDebug() << "Sent data: " << uart_data;
}

void MainWindow::on_DistanceMedium_clicked()
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()
    // Send the distance value for medium distance mode over UART
    QByteArray uart_data = "5";
    UART_SendData(uart_data);

    // Output a message to the console
    qDebug() << "Sent data: " << uart_data;
}

void MainWindow::on_DistanceLong_clicked()
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()
    // Send the distance value for long distance mode over UART
    QByteArray uart_data = "6";
    UART_SendData(uart_data);

    // Output a message to the console
    qDebug() << "Sent data: " << uart_data;
}


void MainWindow::on_checkBox_stateChanged(int state)
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()
    if (state == Qt::Checked)
    {
        // Send the data "A" over UART
        QByteArray uart_data = "A";
        UART_SendData(uart_data);

        // Output a message to the console
        qDebug() << "Sent data: " << uart_data;
    }
}

void MainWindow::on_checkBox_2_stateChanged(int state)
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()
    if (state == Qt::Checked)
    {
        // Send the data "3" over UART
        QByteArray uart_data = "3";
        UART_SendData(uart_data);

        // Output a message to the console
        qDebug() << "Sent data: " << uart_data;
    }
}

void MainWindow::updateDialValue()
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()

    // Receive data over UART
    QByteArray data = UART_ReceiveData(1);

    // Convert the data to an integer value
    int value = *(reinterpret_cast<const int*>(data.constData()));

    // Update the dial value
    ui->Speedometer->setValue(value);
}



void MainWindow::flashRectangle()
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()
    // Read data from the serial port
    QByteArray data = UART_ReceiveData(1);

    // Show the rectangle and make it flash if data is received
    if (!data.isEmpty())
    {
        // Set the visibility of the rectangle to true
        rectangle->setVisible(true);

        // Create a timer that toggles the visibility of the rectangle at a fixed interval
        if (!flashTimer)
        {
            flashTimer = new QTimer(this);
            connect(flashTimer, &QTimer::timeout, this, &MainWindow::toggleRectangleVisibility);
        }
        flashTimer->start(500); // Toggle the visibility of the rectangle every 500 milliseconds
    }
    else
    {
        // Hide the rectangle if no data is received and stop the flashing timer
        rectangle->setVisible(false);
        if (flashTimer)
        {
            flashTimer->stop();
        }
    }
}

void MainWindow::toggleRectangleVisibility()
{
    // Toggle the visibility of the rectangle
    rectangle->setVisible(!rectangle->isVisible());
}

void MainWindow::flashRectangle1()
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()
    // Read data from the serial port
    QByteArray data = UART_ReceiveData(1);

    // Show the rectangle and make it flash if data is received
    if (!data.isEmpty())
    {
        // Set the visibility of the rectangle to true
        rectangle1->setVisible(true);

        // Create a timer that toggles the visibility of the rectangle at a fixed interval
        if (!flashTimer)
        {
            flashTimer = new QTimer(this);
            connect(flashTimer, &QTimer::timeout, this, &MainWindow::toggleRectangleVisibility1);
        }
        flashTimer->start(500); // Toggle the visibility of the rectangle every 500 milliseconds
    }
    else
    {
        // Hide the rectangle if no data is received and stop the flashing timer
        rectangle1->setVisible(false);
        if (flashTimer)
        {
            flashTimer->stop();
        }
    }
}

void MainWindow::toggleRectangleVisibility1()
{
    // Toggle the visibility of the rectangle
    rectangle1->setVisible(!rectangle1->isVisible());
}

void MainWindow::flashRectangle2()
{
    QString device_name = "/dev/ttyAMA0"; // Define the serial port device name
    int baud_rate = 9600;
    UART_Init(device_name, baud_rate); // Call the UART_Init function with the device name and baud rate()
    // Read data from the serial port
    QByteArray data = UART_ReceiveData(1);

    // Set the color and visibility of the rectangle if data is received
    if (!data.isEmpty())
    {
        // Set the color and visibility of the rectangle
        rectangle2->setStyleSheet("background-color: red;");
        rectangle2->setVisible(true);
    }
    else
    {
        // Clear the color and hide the rectangle if no data is received
        rectangle2->setStyleSheet("");
        rectangle2->setVisible(false);
    }
}
