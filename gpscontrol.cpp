#include "gpscontrol.h"
#include <QtNetwork/QNetworkConfigurationManager>

GpsControl::GpsControl(QObject *parent) :
    QObject(parent),
    GpsSettings(this),
    GpsSource(QGeoPositionInfoSource::createDefaultSource(this)),
    GpsPosition(),
    GpsTimeout(this),
    GpsInterval(this) {
    qDebug() << "* GpsControl::GpsControl";

    GpsTimeout.setSingleShot(true);
    GpsInterval.setSingleShot(true);

    setPositioningMethod(GpsSettings.value("gps_method", "all").toString());
    setTimeOut(GpsSettings.value("gps_timeout", 60).toInt());
    setInterval(GpsSettings.value("gps_interval", 15*60).toInt());

    if (!GpsSource) qDebug() << "* GpsControl::GpsControl" << "Not GpsSource";

    if (GpsSource) connect(GpsSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
                           this, SLOT(setCurrentLocation(QGeoPositionInfo)));

    connect(this, SIGNAL(gotUpdate()),
            this, SLOT(onGotUpdate()));

    connect(this, SIGNAL(gotFix()),
            this, SLOT(onGotFix()));

}

void GpsControl::setTimeOut(int sec) {
    qDebug() << "* GpsControl::setTimeOut" << sec;
    GpsSettings.setValue("gps_timeout", sec);
}

int GpsControl::getTimeOut() {
    qDebug() << "* GpsControl::getTimeOut";
    return GpsSettings.value("gps_timeout").toInt();
}

void GpsControl::setInterval(int sec) {
    qDebug() << "* GpsControl::setInterval" << sec;
    GpsSettings.setValue("gps_interval", sec);
}

int GpsControl::getInterval() {
    qDebug() << "* GpsControl::getInterval";
    return GpsSettings.value("gps_interval").toInt();
}

void GpsControl::setPositioningMethod(QString method) {
    qDebug() << "* GpsControl::setPositioningMethod" << method;
    if (method == "gps") {
        GpsSettings.setValue("gps_method", method);
        if (GpsSource) GpsSource->setPreferredPositioningMethods(
                    QGeoPositionInfoSource::SatellitePositioningMethods);
    }
    if (method == "cell") {
        GpsSettings.setValue("gps_method", method);
        if (GpsSource) GpsSource->setPreferredPositioningMethods(
                    QGeoPositionInfoSource::NonSatellitePositioningMethods);
    }
    if (method == "all") {
        GpsSettings.setValue("gps_method", method);
        if (GpsSource) GpsSource->setPreferredPositioningMethods(
                    QGeoPositionInfoSource::AllPositioningMethods);
    }
}

void GpsControl::startUpdates() {
    qDebug() << "* GpsControl::startUpdates";

    GpsTimeout.singleShot(GpsSettings.value("gps_timeout").toInt()*1000,
                          this, SLOT(stopUpdates()));
    GpsInterval.singleShot(GpsSettings.value("gps_interval").toInt()*1000,
                           this, SLOT(startUpdates()));

    QNetworkConfigurationManager mgr;
    if (!GpsSettings.value("net_auto").toBool() && !mgr.isOnline()) {
        qDebug() << "* GpsControl::startUpdates" << "offline";
        return;
    }

    if (GpsSource) GpsSource->startUpdates();
}

void GpsControl::stopUpdates(bool force) {
    qDebug() << "* GpsControl::stopUpdates" << force;

    if (GpsSource) GpsSource->stopUpdates();

    if (force) {
        GpsTimeout.stop();
        GpsInterval.stop();
    } else {
        if (GpsSource && GpsSource->lastKnownPosition(GpsSettings.value("gps_method")
                                         == "gps" ? true : false).isValid() ) {
            emit gotFix();
        }
    }
}

double GpsControl::getCurrentLatitude() {
    qDebug() << "* GpsControl::getCurrentLatitude";
    return GpsPosition.coordinate().latitude();
}

double GpsControl::getCurrentLongitude() {
    qDebug() << "* GpsControl::getCurrentLongitude";
    return GpsPosition.coordinate().longitude();
}

double GpsControl::getCurrentAccuracy() {
    qDebug() << "* GpsControl::getCurrentAccuracy";
    return GpsPosition.attribute(QGeoPositionInfo::HorizontalAccuracy);
}

unsigned int GpsControl::getCurrentTimestamp() {
    qDebug() << "* GpsControl::getCurrentTimestamp";
    return GpsPosition.timestamp().toTime_t();
}

unsigned int GpsControl::getCount() {
    qDebug() << "* GpsControl::getCount";
    static unsigned int i = 0;
    if (i == 9) i = 0;
    return ++i;
}

void GpsControl::setCurrentLocation(QGeoPositionInfo pos) {
    qDebug() << "* GpsControl::setCurrentLocation" << pos;
    if (pos.isValid()) {
        GpsPosition = pos;
        emit gotUpdate();
    }
}
