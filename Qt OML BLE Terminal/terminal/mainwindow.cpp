#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QSplashScreen>
#include <QProgressBar>
#include <QMessageBox>
#include "includes/ble_module.h"
#include "includes/oml_interface.h"
#include "includes/serial.h"
#include "includes/timer.h"
#include "..\..\OML BLE App\mcu_cmds.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <QKeyEvent>
#include "includes/debugsignals.h"


#define MCU_BAUD_RATE 1000000u


extern QSerialPort s_Serial;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , commands()
{
    ui->setupUi(this);
    setWindowTitle("OML BLE Terminal Application");
    ui->textEdit->setReadOnly(false);
    ui->lineEdit->setPlaceholderText("Enter text to transmit");
    ui->lineEdit_2->setPlaceholderText("Enter Node ID to connect");
    ui->pushButton_2->setText("Connect");

    m_checkComPortsTimer = new QTimer(this);
    connect(m_checkComPortsTimer, &QTimer::timeout, this, &MainWindow::checkComPorts);
    m_checkComPortsTimer->start(1000);  // Check every 1 sec

    connect(&s_Serial, &QSerialPort::readyRead, this, &MainWindow::handleReadyRead);
    //connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_sendCommandButton);
    ui->lineEdit->installEventFilter(this);


    connect(&DebugSignals::instance(), &DebugSignals::debugEvent, this, &MainWindow::handleDebugEvent);
    connect(&DebugSignals::instance(), &DebugSignals::debugResponse, this, &MainWindow::handleDebugResponse);
    connect(&DebugSignals::instance(), &DebugSignals::debugHex, this, &MainWindow::handleDebugResponse);
    //connect(&DebugSignals::instance(), &DebugSignals::debugMain, this, &MainWindow::handleDebugEvent);



    initializeCommandMap();

}

MainWindow::~MainWindow()
{
    SerialClose();
    delete ui;
}

bool MainWindow::OMLInterface_Open(uint8_t port)
{
    if (!SerialOpen(port, MCU_BAUD_RATE))
      {
          qDebug() << "Failed to open serial port:" << port;
          return false;
      }

      OMLInterface_Purge();
//      if (!OMLInterface_Wake())
//      {
//          qDebug() << "Failed to wake up the device";
//          SerialClose();
//          return false;
//     }
      return true;
}

void MainWindow::OMLInterface_Purge()
{
    qDebug() <<"Serial data pending?: "+QString::number(SerialRxPending())+". Purging serial buffer";
      while (SerialRxPending())
      {
          (void)SerialReadData();
          qDebug() << "Purged one byte from serial buffer";
      }
      qDebug() << "Completed purging serial buffer";
}

void MainWindow::OMLInterface_Close()
{
     SerialClose();
}

void MainWindow::OMLInterface_Process()
{
    while (SerialRxPending())
    {
        uint8_t ch = SerialReadData();
        qDebug() << "Received data:" << ch;
        BLEModule_OnRx(ch);

        QString str = QString::fromUtf8(reinterpret_cast<char*>(&ch), 1);
        ui->textEdit->moveCursor(QTextCursor::Down);
        ui->textEdit->insertPlainText(str);

    }

}


void MainWindow::OMLInterface_Transmit(const void * const data, size_t len)
{
    SerialWriteBytes(data, (int)len);
}

bool MainWindow::OMLInterface_Wake()
{
    bool ret = false;

    if (SerialClrRts())
    {
        qDebug() << "CLRRTS";
        Sleep(100u);
        if (SerialSetRts())
        {
            qDebug() << "SETRTS";
            Sleep(100u);
            printf("CLRRTS\n");
            if (SerialClrRts())
            {
                qDebug() << "CLRRTS";
                Sleep(100u);
                if (SerialAutoRts())
                {
                    qDebug() << "AUTORTS";
                    ret = true;
                }
                else
                {
                    qDebug() << "Failed to set AUTO RTS";
                }
            }
            else
            {
                qDebug() << "Failed to clear RTS second time";
            }

        }
        else
        {
            qDebug() << "Failed to clear RTS initially";
        }

    }
    else
    {
        qDebug() << "Serial port is not open";
    }
    return ret;


}




void MainWindow::on_pushButton_2_clicked()
{
 /*   bool portOpen = false;

    if (!ui->comboBox->currentText().isEmpty())
    {
        uint8_t port = ui->comboBox->currentText().toUInt();
        portOpen = OMLInterface_Open(port);
    }

    if (portOpen)
    {
        ui->textEdit->setText("COM Port: "+ui->comboBox->currentText()+" Opened OK"+"\n");
        ui->statusbar->showMessage("Connected to Port "+ui->comboBox->currentText());
    }
    else
    {
        ui->textEdit->setText("Failed to Open "+ui->comboBox->currentText()+"\n");
        ui->statusbar->showMessage("Failed to Open "+ui->comboBox->currentText());
    }


    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QString s = QObject::tr("Port: ") + info.portName() + "\n"


                + QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";
        qDebug()<<s;
    }




    TIMER_Init();
    BLEModule_Init();

    OMLInterface_Purge();
    OMLInterface_Process(); */

    if (m_isConnected)
        {
            // Disconnect
            OMLInterface_Close();
            ui->textEdit->setText("COM Port: " + ui->comboBox->currentText() + " Closed OK\n");
            ui->statusbar->showMessage("Disconnected from Port " + ui->comboBox->currentText());
            ui->pushButton_2->setText("Connect");
            m_isConnected = false;
        }
        else
        {
            // Connect
            bool portOpen = false;

            if (!ui->comboBox->currentText().isEmpty())
            {
                uint8_t port = ui->comboBox->currentText().toUInt();
                portOpen = OMLInterface_Open(port);
            }

            if (portOpen)
            {
                ui->textEdit->setText("COM Port: " + ui->comboBox->currentText() + " Opened OK\n");
                ui->statusbar->showMessage("Connected to Port " + ui->comboBox->currentText());
                ui->pushButton_2->setText("Disconnect");
                m_isConnected = true;
            }
            else
            {
                ui->textEdit->setText("Failed to Open " + ui->comboBox->currentText() + "\n");
                ui->statusbar->showMessage("Failed to Open " + ui->comboBox->currentText());
            }

            TIMER_Init();
            BLEModule_Init();

            OMLInterface_Purge();
            OMLInterface_Process();
        }


}

void MainWindow::checkComPorts()
{
    QStringList newComPorts;
    const quint16 targetVID = 0x1915; // Nordic VID
    const quint16 targetPID = 0xFFFF; // specific PID

    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
    {

        if (info.vendorIdentifier() == targetVID && info.productIdentifier() == targetPID)
        {
            newComPorts.append(info.portName());
        }
    }


    for (const QString &port : newComPorts)
    {
        if (!m_currentComPorts.contains(port))
        {
            QString portNumber = port;
            if (port.startsWith("COM"))
            {
                portNumber = port.mid(3);  // Get the part after "COM"
            }
            ui->comboBox->addItem(portNumber);
        }
    }


    QStringList portsToRemove;
    for (int i = 0; i < ui->comboBox->count(); ++i)
    {
        QString portText = "COM" + ui->comboBox->itemText(i);
        if (!newComPorts.contains(portText))
        {
            portsToRemove.append(ui->comboBox->itemText(i));
        }
    }

    for (const QString &port : portsToRemove)
    {
        int index = ui->comboBox->findText(port);
        if (index != -1)
        {
            ui->comboBox->removeItem(index);
        }
    }

    m_currentComPorts = newComPorts;
}

void MainWindow::handleReadyRead()
{

    while (SerialRxPending())
    {
        uint8_t ch = SerialReadData();
        qDebug() << "Received data:" << ch;
        BLEModule_OnRx(ch);
        //         QString str = QString::fromUtf8(reinterpret_cast<char*>(&ch), 1);
        //         ui->textEdit->moveCursor(QTextCursor::Down);
        //         ui->textEdit->insertPlainText(str);
    }
}


void MainWindow::on_pushButton_5_clicked()
{
    commands.nop();
}


void MainWindow::on_pushButton_4_clicked()
{
    commands.fwver();
}


void MainWindow::on_pushButton_6_clicked()
{
    ui->pushButton_6->setEnabled(false);
    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/res/dfuscreen.jpg"));
    splash->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    QProgressBar *progressBar = new QProgressBar(splash);
    progressBar->setGeometry(25, splash->height() - 60, splash->width() - 20, 30);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);

    splash->show();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [progressBar, splash, timer, this]()
    {
        int value = progressBar->value();
        if (value < 100)
        {
            progressBar->setValue(value + 1);
        } else
        {
            timer->stop();
            splash->close();
            delete splash;
            ui->pushButton_6->setEnabled(true);
            QMessageBox::information(nullptr, "OML DFU status", "Firmware update complete");
        }
    });
    timer->start(100); //10 sec


}


void MainWindow::on_pushButton_7_clicked()
{
    commands.onmcureset();
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->textEdit->clear();
    SerialFifoRxPurge();
}

void MainWindow::on_sendCommandButton()
{
    QString command = ui->lineEdit->text().trimmed().toLower();


    if (commandMap.contains(command))
    {
        commandMap[command]();
    }
    else
    {
        ui->textEdit->append("Unknown command: " + command+" :(");
    }

    ui->lineEdit->clear();
}

void MainWindow::handleDebugEvent(const QString &message)
{

    ui->textEdit->setReadOnly(false);
    QTextCursor cursor(ui->textEdit->textCursor());
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();

    QTextCharFormat format;
    format.setForeground(QBrush(QColor("blue")));

    cursor.setCharFormat(format);
    cursor.insertText("Event: " + message);

    ui->textEdit->setTextCursor(cursor);

     QRegularExpression nodeIdRegex("NodeId:(\\d+)");
     QRegularExpression nodeTypeRegex("NodeType:(\\d+)");

     QRegularExpressionMatch nodeIdMatch = nodeIdRegex.match(message);
     QRegularExpressionMatch nodeTypeMatch = nodeTypeRegex.match(message);
     if (!nodeIdMatch.hasMatch() || !nodeTypeMatch.hasMatch()) {
             return; // No NodeId or NodeType found, exit the function
         }

         QString nodeId = nodeIdMatch.captured(1); // Extracted NodeId
         QString nodeType = nodeTypeMatch.captured(1); // Extracted NodeType


         int nodeIdIndex = ui->comboBoxNodeId->findText(nodeId);
         if (nodeIdIndex == -1) {
             // NodeId not in the ComboBox, add it
             ui->comboBoxNodeId->addItem(nodeId);
         }


         int nodeTypeIndex = ui->comboBoxNodeType->findText(nodeType);
         if (nodeTypeIndex == -1) {
             // NodeType not in the ComboBox, add it
             ui->comboBoxNodeType->addItem(nodeType);
         }


}

void MainWindow::handleDebugResponse(const QString &message)
{

    ui->textEdit->setReadOnly(false);
    QTextCursor cursor(ui->textEdit->textCursor());
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();

    QTextCharFormat format;
    format.setForeground(QBrush(QColor("green")));

    cursor.setCharFormat(format);
    cursor.insertText("Response: " + message + "\n");

    ui->textEdit->setTextCursor(cursor);
}

void MainWindow::processInterfaces()
{
    OMLInterface_Process();
}

void MainWindow::initializeCommandMap()
{
    commandMap["nop"] = std::bind(&TerminalCommands::nop, &commands);
    commandMap["onmcureset"] = std::bind(&TerminalCommands::onmcureset, &commands);
    commandMap["getnodeid"] = std::bind(&TerminalCommands::getnodeid, &commands);
    //commandMap["connect"] = std::bind(&TerminalCommands::connectble, &commands);
   // commandMap["disconnect"] = std::bind(&TerminalCommands::disconnectble, &commands);
    commandMap["fwver"] = std::bind(&TerminalCommands::fwver, &commands);
    commandMap["help"] = std::bind(&MainWindow::listAvailableCommands, this);
}

void MainWindow::listAvailableCommands()
{

    ui->textEdit->clear();

    ui->textEdit->append("Available commands:");
    for (auto it = commandMap.cbegin(); it != commandMap.cend(); ++it)
    {
        ui->textEdit->append(it.key());
    }


}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(event->spontaneous()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, "OML Terminal", "Are you sure want to quit Terminal App?\n\nClick Yes to quit or No to remain in this window",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {

            event->accept();

        }
        else if(reply == QMessageBox::No) {
            event->ignore();
        }
    }
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->lineEdit && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            on_sendCommandButton();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_pushButton_8_clicked()
{
    listAvailableCommands();
}


void MainWindow::on_pushButton_9_clicked()
{

    bool ok;
       unsigned int value = ui->lineEdit_2->text().toUInt(&ok, 10);

       if (ok && value >= 1 && value <= 999998) {
           TerminalArg_t args;
           args.l = value;
           commands.connectble(&args);
       } else {

           QMessageBox::warning(this, "Invalid Node Id", "Please enter a valid node Id between 1 and 999998.");
       }
}


void MainWindow::on_pushButton_10_clicked()
{
    TerminalArg_t args;
    args.l = ui->lineEdit_2->text().toUInt(nullptr,10);
  //  args.l = ui->comboBoxNodeId->currentText().toUInt(nullptr, 10);
    commands.disconnectble(&args);
}


void MainWindow::on_pushButton_11_clicked()
{

    TerminalArg_t args[2];
    args[0].l = ui->comboBoxNodeId->currentText().toUInt(nullptr, 10);
    QString payload = ui->lineEdit->text();
    args[1].s = payload.toUtf8().constData();
    commands.txpayload(args);
    ui->lineEdit->clear();
}


void MainWindow::on_pushButton_clicked()
{
    TerminalArg_t args[2];
    args[0].l = ui->comboBoxNodeId->currentText().toUInt(nullptr, 10);
    QString payload = ui->lineEdit->text();
    args[1].s = payload.toUtf8().constData();
    commands.txpayloadack(args);
    ui->lineEdit->clear();
}


void MainWindow::on_pushButton_12_clicked()
{
    QString buildDate = __DATE__;
    QString buildTime = __TIME__;
    QString gitCommitHash = QString(GIT_COMMIT_HASH);


    QString messageText = QString("Software Name: OML BLE Terminal App (WiP)\nVersion: 0.0.2\n"
                                  "Built on: Qt5.12.12 (MSVC 2017, 64 bit) \n\n"
                                  "Copyright Odstock Medical Ltd. All rights reserved.\n\n"
                                  "Built on: %1, %2\n"
                                  "From Git Commit: %3\n")

            .arg(buildDate)
            .arg(buildTime)
            .arg(gitCommitHash);

    QMessageBox msgBox;
    msgBox.setWindowTitle("About");
    msgBox.setText(messageText);
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);
    msgBox.exec();
}
