#include "debugsignals.h"
#include <QString>

extern "C" {

void emitDebugEvent(const char *message)
{
    emit DebugSignals::instance().debugEvent(QString(message));
}

void emitDebugResponse(const char *message)
{
    emit DebugSignals::instance().debugResponse(QString(message));
}


void emitDebugHex(const char *message)
{
    emit DebugSignals::instance().debugHex(QString(message));
}

void emitDebugMain(const char *message)
{
    emit DebugSignals::instance().debugMain(QString(message));
}
}// extern "C"
