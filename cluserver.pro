TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lboost_system -lboost_filesystem -lpthread -lmysqlclient -lmysqlpp
SOURCES += main.cpp \
    boost-server.cpp \
    utils.cpp \
    basecmds.cpp \
    testcmds.cpp \
    basefuncions.cpp \
    service.cpp

HEADERS += \
    boost-server.hpp \
    accountutils.h \
    recarray.h \
    config.h \
    basefuncions.h
