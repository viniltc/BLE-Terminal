#include "mainwindow.h"
#include <QApplication>
#include <QSerialPortInfo>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include "includes/ble_module.h"
#include "includes/oml_interface.h"
#include "includes/serial.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
   // serial_api.openSerialPort(8, 1000000);
    MainWindow w;


    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString s = QObject::tr("Port: ") + info.portName() + "\n"
                + QObject::tr("Location: ") + info.systemLocation() + "\n"
                + QObject::tr("Description: ") + info.description() + "\n"
                + QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
                + QObject::tr("Serial no: ") + info.serialNumber() + "\n"
                + QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
                + QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
                + QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";
        qDebug()<<s;}
 //QObject::connect(serial_api.serial, SIGNAL(readyRead()), &serial_api, SLOT(readData()));

   w.show();
     int result = a.exec();
       qDebug() << "Application exiting with code" << result;

       return result;
}
