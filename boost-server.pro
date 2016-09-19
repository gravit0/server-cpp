TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lboost_system -lboost_filesystem -lpthread -lmysqlclient
SOURCES += main.cpp \
    boost-server.cpp \
    utils.cpp \
    databasehelper.cpp \
    basecmds.cpp

HEADERS += \
    main.hpp \
    boost-server.hpp \
    abstractcommand.h \
    accountutils.h \
    databasehelper.h
