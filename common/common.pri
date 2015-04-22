TEMPLATE = app
TARGET = $${TARGET}.elf
CONFIG = c++11

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
