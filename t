#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QSpiDevice>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create a QWidget to hold the UI elements
    QWidget widget;

    // Create a QVBoxLayout to hold the UI elements vertically
    QVBoxLayout layout(&widget);

    // Create a QLabel to display instructions
    QLabel label("Enter a number and press the button to send it over SPI:");
    layout.addWidget(&label);

    // Create a QLineEdit to input the number
    QLineEdit lineEdit;
    layout.addWidget(&lineEdit);

    // Create a QPushButton to trigger the sending process
    QPushButton sendButton("Send over SPI");
    layout.addWidget(&sendButton);

    // Create a QSpiDevice to communicate with the SPI bus
    QSpiDevice spi("/dev/spidev0.0");
    spi.setClock(1000000); // Set the clock speed to 1 MHz

    // Connect the sendButton's clicked() signal to a lambda function that sends the number over SPI
    QObject::connect(&sendButton, &QPushButton::clicked, [&]() {
        // Get the number from the lineEdit and convert it to an integer
        QString numberString = lineEdit.text();
        bool ok;
        int number = numberString.toInt(&ok);
        if (!ok) {
            qDebug() << "Invalid number entered!";
            return;
        }

        // Pack the number into a buffer (assumes little-endian byte order)
        uint8_t buffer[4];
        buffer[0] = number & 0xFF;
        buffer[1] = (number >> 8) & 0xFF;
        buffer[2] = (number >> 16) & 0xFF;
        buffer[3] = (number >> 24) & 0xFF;

        // Send the buffer over SPI
        spi.write(buffer, 4);
    });

    // Show the widget
    widget.show();

    // Run the application event loop
    return a.exec();
}





void MainWindow::updateConsole()
{
    // Receive data from SPI and append to the Console list view
    uint8_t received = SPI_ReceiveData();
    QString received_str = QString("Received data: %1").arg(received);

    // Create a new item for the received data
    QStandardItem* item = new QStandardItem(received_str);

    // Add the item to the model for the Console list view
    ui->Console->model()->appendRow(item);
}
