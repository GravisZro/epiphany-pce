TEMPLATE = app

!include( ../common/common.pri ) {
    error( Could not find the common.pri file! )
}

SOURCES += main.cpp \
    6270core.cpp

HEADERS += \
    6270core.h \
    6270registers.h
