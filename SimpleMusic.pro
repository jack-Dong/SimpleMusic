#-------------------------------------------------
#
# Project created by QtCreator 2016-12-31T14:45:45
#
#-------------------------------------------------

QT       += core gui sql multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimpleMusic
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    mysqldb.cpp

HEADERS  += widget.h \
    mysqldb.h

FORMS    += widget.ui

RESOURCES += \
    img.qrc
