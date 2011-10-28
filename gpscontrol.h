#ifndef GPSCONTROL_H
#define GPSCONTROL_H

#include <QObject>
#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtLocation/QGeoPositionInfo>
#include <QtLocation/QGeoPositionInfoSource>

using namespace QtMobility;

class GpsControl : public QObject {
    Q_OBJECT
public:
    GpsControl(QObject *parent = 0);

signals:
    void gotUpdate();
    void gotFix();

public slots:
    void setTimeOut(int sec);
    int getTimeOut();
    void setInterval(int sec);
    int getInterval();
    void setPositioningMethod(QString method);
    QString getPositioningMethod() {return GpsSettings.value("gps_method").toString();}
    void startUpdates();
    void stopUpdates(bool force = false);
    double getCurrentLatitude();
    double getCurrentLongitude();
    double getCurrentAccuracy();
    unsigned int getCurrentTimestamp();

private slots:
    void setCurrentLocation(QGeoPositionInfo pos);
    void onGotUpdate() { qDebug() << "* GpsControl::ongotUpdate"; }
    void onGotFix() { qDebug() << "* GpsControl::ongotFix"; }

private:
    QSettings GpsSettings;
    QGeoPositionInfoSource *GpsSource;
    QGeoPositionInfo GpsPosition;
    QTimer GpsTimeout;
    QTimer GpsInterval;
};

#endif // GPSCONTROL_H
