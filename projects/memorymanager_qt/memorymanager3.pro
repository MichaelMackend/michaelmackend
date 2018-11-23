TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    memoryallocator.cpp \
    bar.cpp \
    backtrace.cpp

HEADERS += \
    bar.h \
    backtrace.h \
    mallocator11.h \
    memoryallocator.h
