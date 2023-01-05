#//////////////////////////////////////////////////
#//                                              //
#// CasToSio                                     //
#// by  Thorsten Kattanek                        //
#//						 //
#// Atari Cas files send over sio bus with uart  //
#//                                              //
#// #file: src.pro                               //
#//                                              //
#// This source code is Copyright protected!     //
#//                                              //
#// Last changed at 2022-12-29                   //
#// https://github.com/ThKattanek/cas_to_sio     //
#//                                              //
#//////////////////////////////////////////////////

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = castosio
TEMPLATE = app

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cas_file_class.cpp \
    main.cpp \
    mainwindow.cpp \
    sio_transmit_thread.cpp

HEADERS += \
    cas_file_class.h \
    mainwindow.h \
    qutils.h \
    sio_transmit_thread.h

FORMS += \
    mainwindow.ui

LIBS += \
    -lserialport

win32 {
    PKGCONFIG += quazip
} else {
    LIBS += -lquazip5
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
