QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
# In your .pro file
git_commit_hash = $$system(git rev-parse --short HEAD)
DEFINES += GIT_COMMIT_HASH=\\\"$$git_commit_hash\\\"

SOURCES += \
    includes/ble_module.c \
    includes/crc8.c \
    includes/debug.c \
    includes/debug_signals_wrapper.cpp \
    includes/debugsignals.cpp \
    includes/oml_interface.c \
    includes/serial.cpp \
    includes/terminalcommands.cpp \
    includes/timer.c \
    includes/utils.c \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    includes/ble_module.h \
    includes/crc8.h \
    includes/debug.h \
    includes/debug_signals_wrapper.h \
    includes/debugsignals.h \
    includes/oml_interface.h \
    includes/serial.h \
    includes/terminalcommands.h \
    includes/timer.h \
    includes/utils.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    res/dfuscreen.jpg
