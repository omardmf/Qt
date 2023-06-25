#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "ui_mainwindow.h"
#include "qwt_plot.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void UART_Init(const QString& device_name, int baud_rate);

    void UART_SendData(const QByteArray& data);

    QByteArray UART_ReceiveData(int length) ;

    void updateDialValue();

    void flashRectangle();

    void toggleRectangleVisibility();

    void flashRectangle1();

    void toggleRectangleVisibility1();

    void flashRectangle2();

private slots:

    void on_SetSpeed_clicked();

    void on_SpeedIncrement_clicked();

    void on_SpeedDecrement_clicked();

    void on_NomralMode_clicked();

    void on_ACCMode_clicked();

    void on_DistanceShort_clicked();

    void on_DistanceMedium_clicked();

    void on_DistanceLong_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QLabel *rectangle;
    QLabel *rectangle1;
    QLabel *rectangle2;
    QTimer* flashTimer = nullptr;
};
#endif // MAINWINDOW_H
