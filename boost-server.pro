TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lboost_system -lboost_filesystem -lpthread
SOURCES += main.cpp \
    boost-server.cpp

HEADERS += \
    main.hpp \
    boost-server.hpp \
    abstractcommand.h
