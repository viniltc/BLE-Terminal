
#include "serial.h"
#include <QString>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

QSerialPort s_Serial;

#ifdef __cplusplus
extern "C" {
#endif

bool SerialOpen(uint8_t nPort, int nBaud)
{
    QString portName = QString("COM%1").arg(nPort);
    s_Serial.setPortName(portName);
    s_Serial.setBaudRate(nBaud);
    s_Serial.setDataBits(QSerialPort::Data8);
    s_Serial.setParity(QSerialPort::NoParity);
    s_Serial.setStopBits(QSerialPort::OneStop);
    s_Serial.setFlowControl(QSerialPort::NoFlowControl);
    //  return s_Serial.open(QIODevice::ReadWrite);

    if (s_Serial.open(QIODevice::ReadWrite)) {
        s_Serial.setRequestToSend(true);  // Set RTS
        s_Serial.setDataTerminalReady(true);  // Set DTR
        qDebug() << "Opened port:" << portName << "at baud rate:" << nBaud;
        return true;
    } else {
        qDebug() << "Failed to open port:" << portName;
        return false;
    }
}

void SerialClose(void)
{
    if (s_Serial.isOpen()) {
           qDebug() << "Closing serial port.";
           s_Serial.close();
           qDebug() << "Serial port closed.";
       }
       else {
           qDebug() << "Serial port was not open.";
       }
}

bool SerialWriteByte(uint8_t u8Byte)
{
    if (s_Serial.isOpen()) {
        return (s_Serial.write(reinterpret_cast<const char*>(&u8Byte), 1) == 1);
    }
    return false;
}

void SerialWriteBytes(const void *p, size_t len)
{
    if (s_Serial.isOpen()) {
        s_Serial.write(reinterpret_cast<const char*>(p), len);
    }
}

bool SerialWriteString(char *pszText)
{
    if (s_Serial.isOpen()) {
        return (s_Serial.write(pszText, strlen(pszText)) == static_cast<qint64>(strlen(pszText)));
    }
    return false;
}

bool SerialRxPending(void)
{
    return s_Serial.bytesAvailable() > 0;
}

void SerialFifoRxPurge(void)
{
    if (s_Serial.isOpen()) {
        qDebug() << "Clearing input buffer.";
        s_Serial.clear(QSerialPort::Input);
    }
    else
    {
        qDebug() << "Serial port is not open. Cannot clear input buffer.";
    }
}

void SerialWriteData(uint8_t u8Data)
{
    SerialWriteByte(u8Data);
}

uint8_t SerialReadData(void)
{
    uint8_t byte = 0;
    if (s_Serial.isOpen() && s_Serial.bytesAvailable() > 0) {
        s_Serial.read(reinterpret_cast<char*>(&byte), 1);
    }
    return byte;
}

bool SerialSetRts(void)
{
    if (s_Serial.isOpen()) {
        return s_Serial.setRequestToSend(true);
    }
    return false;
}

bool SerialClrRts(void)
{
    if (s_Serial.isOpen()) {
        return s_Serial.setRequestToSend(false);
    }
    return false;
}

bool SerialAutoRts(void)
{
    // QSerialPort does not support auto RTS directly,
    return false;


}

//QSerialPort& getSerialPort() {
//    return s_Serial;
//}


#ifdef __cplusplus
}
#endif

