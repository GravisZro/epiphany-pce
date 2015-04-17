TEMPLATE = app
TARGET = $${TARGET}.elf
CONFIG = c++11
#QMAKESPEC = epiphany-g++

#QMAKE_CXXFLAGS += -Os
#QMAKE_CXXFLAGS += -Wno-unused-parameter
#QMAKE_CXXFLAGS += -finline-functions
#QMAKE_CXXFLAGS += -finline-functions-called-once
#QMAKE_CXXFLAGS += -fconserve-stack
#QMAKE_CXXFLAGS += -fcaller-saves
#QMAKE_CXXFLAGS += -fomit-frame-pointer
OBJECTS_DIR = .obj

#QMAKE_CXXFLAGS += -S

#DEFINES += MED_MEM_DEBUG
#DEFINES += DEBUG_ENABLED
DEFINES += DEBUG_CORE_ONLY
DEFINES += DEBUG_SILENT
DEFINES += TOLERANT_ADDRESSING

INCLUDEPATH += ../common

HEADERS += \
    ../common/global.h \
    ../common/businterface.h \
    ../common/cores.h

SOURCES += \
    ../common/global.cpp \
    ../common/businterface.cpp

QMAKE_POST_LINK=e-strip --strip-all $${TARGET};e-objcopy --srec-forceS3 --output-target srec $${TARGET} $${TARGET}.srec;
