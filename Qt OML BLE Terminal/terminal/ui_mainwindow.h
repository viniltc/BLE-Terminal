/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QComboBox *comboBox;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButton_2;
    QTextEdit *textEdit;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_12;
    QPushButton *pushButton_5;
    QPushButton *pushButton_4;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_2;
    QComboBox *comboBoxNodeId;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_3;
    QComboBox *comboBoxNodeType;
    QHBoxLayout *horizontalLayout_9;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;
    QHBoxLayout *horizontalLayout_11;
    QLineEdit *lineEdit;
    QPushButton *pushButton_11;
    QPushButton *pushButton;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_3;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(600, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(centralwidget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        horizontalLayout->addWidget(comboBox);


        horizontalLayout_6->addLayout(horizontalLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));

        horizontalLayout_6->addLayout(horizontalLayout_5);

        horizontalSpacer_3 = new QSpacerItem(65, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_3);

        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        horizontalLayout_6->addWidget(pushButton_2);


        gridLayout->addLayout(horizontalLayout_6, 0, 0, 1, 2);

        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        gridLayout->addWidget(textEdit, 1, 0, 1, 2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pushButton_12 = new QPushButton(centralwidget);
        pushButton_12->setObjectName(QString::fromUtf8("pushButton_12"));

        horizontalLayout_4->addWidget(pushButton_12);

        pushButton_5 = new QPushButton(centralwidget);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));

        horizontalLayout_4->addWidget(pushButton_5);

        pushButton_4 = new QPushButton(centralwidget);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));

        horizontalLayout_4->addWidget(pushButton_4);

        pushButton_6 = new QPushButton(centralwidget);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));

        horizontalLayout_4->addWidget(pushButton_6);

        pushButton_7 = new QPushButton(centralwidget);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));

        horizontalLayout_4->addWidget(pushButton_7);


        gridLayout->addLayout(horizontalLayout_4, 3, 0, 1, 2);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_7->addWidget(label_2);

        comboBoxNodeId = new QComboBox(centralwidget);
        comboBoxNodeId->setObjectName(QString::fromUtf8("comboBoxNodeId"));

        horizontalLayout_7->addWidget(comboBoxNodeId);


        horizontalLayout_10->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_8->addWidget(label_3);

        comboBoxNodeType = new QComboBox(centralwidget);
        comboBoxNodeType->setObjectName(QString::fromUtf8("comboBoxNodeType"));

        horizontalLayout_8->addWidget(comboBoxNodeType);


        horizontalLayout_10->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        lineEdit_2 = new QLineEdit(centralwidget);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        horizontalLayout_9->addWidget(lineEdit_2);

        pushButton_9 = new QPushButton(centralwidget);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));

        horizontalLayout_9->addWidget(pushButton_9);

        pushButton_10 = new QPushButton(centralwidget);
        pushButton_10->setObjectName(QString::fromUtf8("pushButton_10"));

        horizontalLayout_9->addWidget(pushButton_10);


        horizontalLayout_10->addLayout(horizontalLayout_9);


        gridLayout->addLayout(horizontalLayout_10, 4, 0, 1, 2);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout_11->addWidget(lineEdit);

        pushButton_11 = new QPushButton(centralwidget);
        pushButton_11->setObjectName(QString::fromUtf8("pushButton_11"));

        horizontalLayout_11->addWidget(pushButton_11);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout_11->addWidget(pushButton);


        gridLayout->addLayout(horizontalLayout_11, 5, 1, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer_2 = new QSpacerItem(617, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        pushButton_3 = new QPushButton(centralwidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        horizontalLayout_3->addWidget(pushButton_3);


        gridLayout->addLayout(horizontalLayout_3, 2, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QApplication::translate("MainWindow", "OML BLE Dongle COM Port", nullptr));
        pushButton_2->setText(QApplication::translate("MainWindow", "Connect", nullptr));
        pushButton_12->setText(QApplication::translate("MainWindow", "About", nullptr));
        pushButton_5->setText(QApplication::translate("MainWindow", "NOP", nullptr));
        pushButton_4->setText(QApplication::translate("MainWindow", "Firmware Version", nullptr));
        pushButton_6->setText(QApplication::translate("MainWindow", "Firmware update (Demo)", nullptr));
        pushButton_7->setText(QApplication::translate("MainWindow", "On MCU Reset", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Available Nodes", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Available Node Type", nullptr));
        pushButton_9->setText(QApplication::translate("MainWindow", "Connect Node", nullptr));
        pushButton_10->setText(QApplication::translate("MainWindow", "Disconnect Node", nullptr));
        pushButton_11->setText(QApplication::translate("MainWindow", "Tx Payload", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "Tx Payload Ack ", nullptr));
        pushButton_3->setText(QApplication::translate("MainWindow", "Clr Terminal", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
