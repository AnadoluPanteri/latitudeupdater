TEMPLATE = lib
CONFIG   += staticlib
TARGET   = kqoauth

QT       -= gui
QT       += network

HEADERS += kqoauthauthreplyserver.h \
           kqoauthauthreplyserver_p.h \
           kqoauthglobals.h \
           kqoauthmanager.h \
           kqoauthmanager_p.h \
           kqoauthrequest.h \
           kqoauthrequest_1.h \
           kqoauthrequest_p.h \
           kqoauthrequest_xauth.h \
           kqoauthrequest_xauth_p.h \
           kqoauthutils.h
SOURCES += kqoauthauthreplyserver.cpp \
           kqoauthmanager.cpp \
           kqoauthrequest.cpp \
           kqoauthrequest_1.cpp \
           kqoauthrequest_xauth.cpp \
           kqoauthutils.cpp

target.path = /opt/linfati.com/lib
INSTALLS += target
