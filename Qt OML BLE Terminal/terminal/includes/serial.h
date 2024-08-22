#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
#include <QSerialPort>

extern QSerialPort s_Serial;  // Extern declaration
#endif

#ifdef __cplusplus
extern "C" {
#endif



bool SerialOpen(uint8_t nPort, int nBaud);
void SerialClose(void);
bool SerialWriteByte(uint8_t u8Byte);
void SerialWriteBytes(const void *p, size_t len);
bool SerialWriteString(char *pszText);
bool SerialRxPending(void);
void SerialFifoRxPurge(void);
void SerialWriteData(uint8_t u8Data);
uint8_t SerialReadData(void);
bool SerialSetRts(void);
bool SerialClrRts(void);
bool SerialAutoRts(void);
//QSerialPort& getSerialPort();


#ifdef __cplusplus
}
#endif

#endif // SERIAL_H
