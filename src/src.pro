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
#// Last changed at 2023-01-22                   //
#// https://github.com/ThKattanek/cas_to_sio     //
#//                                              //
#//////////////////////////////////////////////////

!win32:isEmpty(PREFIX):PREFIX=/usr/local

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = castosio
TEMPLATE = app

CONFIG += c++17
CONFIG += object_parallel_to_source

CASTOSIO_VERSION = 0.1.0

# Versionsnummer ermitteln aus Git Tag Nummer
GIT_VERSION = $$system(git --git-dir \"../.git\" describe --always --tags)
isEmpty(GIT_VERSION) {
  GIT_VERSION = $$CASTOSIO_VERSION
}
DEFINES += VERSION_STRING=\\\"$$GIT_VERSION\\\"

message("CasToSio Version: " $$GIT_VERSION)

contains(QT_ARCH, x86_64){
    CASTOSIO_ARCH = 64Bit
} else:contains(QT_ARCH, i[3456]86) {
    CASTOSIO_ARCH = 32Bit
} else {
    CASTOSIO_ARCH = Unknown
}

# Compilerflags
QMAKE_CXXFLAGS+=-Wno-multichar

DEFINES += ARCHITECTURE_STRING=\\\"$$CASTOSIO_ARCH\\\"

message("CasToSio Architecture: " $$CASTOSIO_ARCH)

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

win32 {
    CONFIG += link_pkgconfig
    PKGCONFIG += quazip
    PKGCONFIG += libserialport
} else {
    LIBS += -lquazip5 -lserialport
}

# Installation

message(Installpath: $$PREFIX)
DEFINES += DATA_PATH=\\\"$$PREFIX\\\"

win32 {
    target.path = $$PREFIX
    txt.path = $$PREFIX
} else {
    target.path = $$PREFIX/bin
    txt.path = $$PREFIX/share/doc/$$TARGET
}

# TXT
txt.CONFIG += nostrip
txt.files += ../LICENSE

#INSTALLS += target txt

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target txt
