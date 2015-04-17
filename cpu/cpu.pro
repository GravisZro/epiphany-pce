TEMPLATE = app

!include( ../common/common.pri ) {
    error( Could not find the common.pri file! )
}

SOURCES += main.cpp \
    6280core.cpp \
    debug.cpp \
    6280irqs.cpp \
    6280timer.cpp

HEADERS += \
    6280core.h \
    6280registers.h \
    debug.h \
    6280irqs.h \
    6280timer.h

