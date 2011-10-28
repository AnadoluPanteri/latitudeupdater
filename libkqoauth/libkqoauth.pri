QT       += network

INCLUDEPATH += libkqoauth

HEADERS += libkqoauth/QtKOAuth \
           libkqoauth/kqoauthmanager.h \
           libkqoauth/kqoauthrequest.h \
           libkqoauth/kqoauthrequest_1.h \
           libkqoauth/kqoauthrequest_xauth.h \
           libkqoauth/kqoauthglobals.h \
           libkqoauth/kqoauthrequest_p.h \
           libkqoauth/kqoauthmanager_p.h \
           libkqoauth/kqoauthauthreplyserver.h \
           libkqoauth/kqoauthauthreplyserver_p.h \
           libkqoauth/kqoauthutils.h \
           libkqoauth/kqoauthrequest_xauth_p.h

SOURCES += libkqoauth/kqoauthauthreplyserver.cpp \
           libkqoauth/kqoauthmanager.cpp \
           libkqoauth/kqoauthrequest.cpp \
           libkqoauth/kqoauthrequest_1.cpp \
           libkqoauth/kqoauthrequest_xauth.cpp \
           libkqoauth/kqoauthutils.cpp

OTHER_FILES += libkqoauth/no_desktopservice.patch
