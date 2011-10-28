contains(QT_CONFIG, maemo5): {
message(maemo5 qtmobility12 backported)
QT          += maemo5
CONFIG      += link_pkgconfig
PKGCONFIG   += glib-2.0 liblocation

HEADERS += liblocationmaemo5/gconfitem_p.h \
           liblocationmaemo5/liblocationwrapper_p.h \
           liblocationmaemo5/qgeopositioninfosource_maemo5_p.h \
           liblocationmaemo5/qgeosatelliteinfosource_maemo5_p.h
SOURCES += liblocationmaemo5/gconfitem.cpp \
           liblocationmaemo5/liblocationwrapper.cpp \
           liblocationmaemo5/qgeopositioninfosource_maemo5.cpp \
           liblocationmaemo5/qgeosatelliteinfosource_maemo5.cpp
}
