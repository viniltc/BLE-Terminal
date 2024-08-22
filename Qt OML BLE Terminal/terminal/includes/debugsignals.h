#ifndef DEBUGSIGNALS_H
#define DEBUGSIGNALS_H

#include <QObject>
#include <QString>

class DebugSignals : public QObject
{
    Q_OBJECT

public:
    static DebugSignals& instance()
    {
        static DebugSignals instance;
        return instance;
    }

signals:
    void debugEvent(const QString &message);
    void debugResponse(const QString &message);
    void debugHex(const QString &message);
    void debugMain(const QString &message);

private:
    DebugSignals() {}  // Private constructor for singleton pattern
    Q_DISABLE_COPY(DebugSignals)
};

#endif // DEBUGSIGNALS_H
