#ifndef DEBUG_SIGNALS_WRAPPER_H
#define DEBUG_SIGNALS_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void emitDebugEvent(const char *message);
void emitDebugResponse(const char *message);
void emitDebugHex(const char *message);
void emitDebugMain(const char *message);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_SIGNALS_WRAPPER_H
