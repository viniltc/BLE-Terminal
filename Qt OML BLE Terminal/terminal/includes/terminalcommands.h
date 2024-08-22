#ifndef TERMINALCOMMANDS_H
#define TERMINALCOMMANDS_H

#include <QObject>
#include "includes/ble_module.h"
#include "includes/oml_interface.h"
#include "includes/serial.h"
#include "..\..\OML BLE App\mcu_cmds.h"
#include "..\..\OML BLE App\types.h"
#include "..\..\OML BLE App\utils.h"

typedef union {
   const char *s;// Arguments can be string, uint16_t or uint32_t
   int16_t i;
   uint32_t l;
} TerminalArg_t;

class TerminalCommands
{
public:
    TerminalCommands();

    void nop();
    void onmcureset();
    void getnodeid();
    void fwver();
    void connectble(TerminalArg_t *args);
    void disconnectble(TerminalArg_t *args);
    void txpayload(TerminalArg_t *args);
    void txpayloadack(TerminalArg_t *args);

};

#endif // TERMINALCOMMANDS_H
