TEMPLATE = app

!include( ../common/common.pri ) {
    error( Could not find the common.pri file! )
}

SOURCES += main.cpp \
    6260core.cpp

HEADERS += \
    6260core.h \
    6260registers.h

