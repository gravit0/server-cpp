TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lboost_system -lboost_filesystem -lpthread -lmysqlclient -lmysqlpp
SOURCES += src/main.cpp \
    src/boost-server.cpp \
    src/utils.cpp \
    src/basecmds.cpp \
    src/testcmds.cpp \
    src/basefuncions.cpp \
    src/service.cpp \
    src/tcpclient.cpp \
    src/memcached.cpp
INCLUDEPATH += "include"
HEADERS += \
    include/boost-server.hpp \
    include/accountutils.h \
    include/recarray.h \
    include/config.h \
    include/basefuncions.h \
    include/tcpclient.h \
    include/memcached.h
