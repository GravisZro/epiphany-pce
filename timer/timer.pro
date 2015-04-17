TEMPLATE = app

!include( ../common/common.pri ) {
    error( Could not find the common.pri file! )
}

SOURCES += main.cpp \
    6280timer.cpp

HEADERS += \
    6280timer.h

