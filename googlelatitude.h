#ifndef GOOGLELATITUDE_H
#define GOOGLELATITUDE_H

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QtDebug>
#include <QtKOAuth>

class GoogleLatitude : public QObject {
    Q_OBJECT

public:
    GoogleLatitude(QObject *parent = 0);
    ~GoogleLatitude();

public slots:
    void getAccess();
    void getCurrentLocation();
    void sendCurrentLocation();
    void setCurrentLocation(double la, double lo, double ac);
    QUrl getUserAuthorization();
    void setAutoConnect(bool status);
    bool getAutoConnect();
    void setDaemonMode(bool status);
    bool getDaemonMode();

private slots:
    void onTemporaryTokenReceived(QString temporaryToken, QString temporaryTokenSecret);
    void onAuthorizationReceived(QString token, QString verifier);
    void onAccessTokenReceived(QString token, QString tokenSecret);
    void onRequestReady(QByteArray response);
    void onAuthorizedRequestDone() { qDebug() << "* GoogleLatitude::onAuthorizedRequestDone"; }
    void onNotToken() { qDebug() << "* GoogleLatitude::onNotToken"; }
    void onGotToken() { qDebug() << "* GoogleLatitude::onGotToken"; }
    void onNeedAuth() { qDebug() << "* GoogleLatitude::onNeedAuth"; }

signals:
    void notToken();
    void gotToken();
    void needAuth();

private:
    QSettings OauthSettings;
    QUrl OAuthGetRequestToken;
    QUrl OAuthAuthorizeToken;
    QUrl OAuthGetAccessToken;
    QUrl CurrentLocation;
    QUrl UserAuthorization;
    KQOAuthParameters GoogleOauthAdditional;
    QString ConsumerKey;
    QString ConsumerSecretKey;
    double CurrentLatitude;
    double CurrentLongitude;
    double CurrentAccuracy;
    KQOAuthManager *OauthManager;
    KQOAuthRequest *OauthRequest;
};

#endif // GOOGLELATITUDE_H
