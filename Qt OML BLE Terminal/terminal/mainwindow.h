#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QGroupBox>
#include "includes/terminalcommands.h"
#include <functional>
#include <string>
#include <map>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool OMLInterface_Open(uint8_t port);
    void OMLInterface_Purge(void);
    void OMLInterface_Close(void);
    void OMLInterface_Process(void);
    void OMLInterface_Transmit(const void *const data, size_t len);
    bool OMLInterface_Wake(void);
    void checkComPorts();

public slots:
    void handleReadyRead();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override; // Event filter



private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_3_clicked();

    void on_sendCommandButton();

    void handleDebugEvent(const QString &message);
    void handleDebugResponse(const QString &message);
    void  processInterfaces();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_clicked();

    void on_pushButton_12_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *m_serialPort;
    QTimer *m_checkComPortsTimer;
    QTimer *m_processTimer;
    QStringList m_currentComPorts;
//    uint32_t baud = 1000000;
    TerminalCommands commands;
     bool m_isConnected = false;

    typedef std::function<void()> CommandFunction;
    QMap<QString, CommandFunction> commandMap;
    void initializeCommandMap();
    void listAvailableCommands();
    void closeEvent (QCloseEvent *event);

//    bool nop();
//    bool onmcureset();
//    bool getnodeid();
//    bool connectble();
//    bool disconnectble();
//    bool fwver();



};
#endif // MAINWINDOW_H
