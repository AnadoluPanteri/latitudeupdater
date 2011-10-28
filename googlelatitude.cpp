#include "googlelatitude.h"
#include <QtNetwork/QNetworkConfigurationManager>

GoogleLatitude::GoogleLatitude(QObject *parent) :
    QObject(parent),
    OauthSettings(this),
    OAuthGetRequestToken("https://www.google.com/accounts/OAuthGetRequestToken"),
    OAuthAuthorizeToken("https://www.google.com/accounts/OAuthAuthorizeToken"),
    OAuthGetAccessToken("https://www.google.com/accounts/OAuthGetAccessToken"),
    CurrentLocation("https://www.googleapis.com/latitude/v1/currentLocation"),
    UserAuthorization(""),
    ConsumerKey("1062862865804.apps.googleusercontent.com"),
    ConsumerSecretKey("EYQaRaUJ9Fznw8mPMor660Kx"),
    CurrentLatitude(0),
    CurrentLongitude(0),
    CurrentAccuracy(0) {
    qDebug() << "* GoogleLatitude::GoogleLatitude";
    OauthRequest = new KQOAuthRequest(this);
    OauthManager = new KQOAuthManager(this);
    GoogleOauthAdditional.insert("scope", "https://www.googleapis.com/auth/latitude");
    GoogleOauthAdditional.insert("xoauth_displayname", "LatitudeUpdater");

    connect(OauthManager, SIGNAL(temporaryTokenReceived(QString,QString)),
            this, SLOT(onTemporaryTokenReceived(QString, QString)));

    connect(OauthManager, SIGNAL(authorizationReceived(QString,QString)),
            this, SLOT(onAuthorizationReceived(QString, QString)));

    connect(OauthManager, SIGNAL(accessTokenReceived(QString,QString)),
            this, SLOT(onAccessTokenReceived(QString,QString)));

    connect(OauthManager, SIGNAL(requestReady(QByteArray)),
            this, SLOT(onRequestReady(QByteArray)));

    connect(OauthManager, SIGNAL(authorizedRequestDone()),
            this, SLOT(onAuthorizedRequestDone()));

    connect(this, SIGNAL(gotToken()),
            this, SLOT(getCurrentLocation()));

    connect(this, SIGNAL(notToken()),
            this, SLOT(onNotToken()));

    connect(this, SIGNAL(gotToken()),
            this, SLOT(onGotToken()));

    connect(this, SIGNAL(needAuth()),
            this, SLOT(onNeedAuth()));
}

GoogleLatitude::~GoogleLatitude() {
    qDebug() << "* GoogleLatitude::~GoogleLatitude";
    delete OauthManager;
    delete OauthRequest;
}

void GoogleLatitude::getAccess() {
    qDebug() << "* GoogleLatitude::getAccess";
    if ( OauthSettings.value("oauth_token").isNull() || OauthSettings.value("oauth_token_secret").isNull() ) {
        OauthRequest->clearRequest();
        OauthRequest->initRequest(KQOAuthRequest::TemporaryCredentials, OAuthGetRequestToken);
        OauthRequest->setConsumerKey(ConsumerKey);
        OauthRequest->setConsumerSecretKey(ConsumerSecretKey);
        OauthRequest->setAdditionalParameters(GoogleOauthAdditional);
        OauthManager->setHandleUserAuthorization(true);
        OauthRequest->setHttpMethod(KQOAuthRequest::POST);
        OauthManager->executeRequest(OauthRequest);
    } else {
        emit gotToken();
    }
}

void GoogleLatitude::getCurrentLocation() {
    qDebug() << "* GoogleLatitude::getCurrentLocation";
    if ( OauthSettings.value("oauth_token").isNull() || OauthSettings.value("oauth_token_secret").isNull() ) {
        emit notToken();
    } else {
        OauthRequest->clearRequest();
        OauthRequest->initRequest(KQOAuthRequest::AuthorizedRequest, CurrentLocation);
        OauthRequest->setToken(OauthSettings.value("oauth_token").toString());
        OauthRequest->setTokenSecret(OauthSettings.value("oauth_token_secret").toString());
        OauthRequest->setConsumerKey(ConsumerKey);
        OauthRequest->setConsumerSecretKey(ConsumerSecretKey);
        OauthRequest->setHttpMethod(KQOAuthRequest::GET);
        OauthManager->executeRequest(OauthRequest);
    }
}

void GoogleLatitude::sendCurrentLocation() {
    qDebug() << "* GoogleLatitude::sendCurrentLocation";
    if (OauthSettings.value("oauth_token").isNull() || OauthSettings.value("oauth_token_secret").isNull()) {
        emit notToken();
    } else {
        if (abs(CurrentLatitude) <= 0.01) return;
        if (abs(CurrentLongitude) <= 0.01) return;
        if (abs(CurrentAccuracy) <= 0.01) return;

        QNetworkConfigurationManager mgr;
        if (!OauthSettings.value("net_auto").toBool() && !mgr.isOnline()) {
            qDebug() << "* GoogleLatitude::sendCurrentLocation" << "offline";
            return;
        }

        QByteArray json_location;
        json_location = "{\"data\": {\"kind\":\"latitude#location\",";
        json_location += QString("\"latitude\":%1,").arg(CurrentLatitude);
        json_location += QString("\"longitude\":%1,").arg(CurrentLongitude);
        json_location += QString("\"accuracy\":%1").arg(CurrentAccuracy);
        json_location += "}}";
        qDebug() << "json_location" <<  json_location;

        OauthRequest->clearRequest();
        OauthRequest->initRequest(KQOAuthRequest::AuthorizedRequest, CurrentLocation);
        OauthRequest->setToken(OauthSettings.value("oauth_token").toString());
        OauthRequest->setTokenSecret(OauthSettings.value("oauth_token_secret").toString());
        OauthRequest->setConsumerKey(ConsumerKey);
        OauthRequest->setConsumerSecretKey(ConsumerSecretKey);
        OauthRequest->setHttpMethod(KQOAuthRequest::POST);
        OauthRequest->setContentType("application/json");
        OauthRequest->setRawData(json_location);
        OauthManager->executeRequest(OauthRequest);
    }
}

void GoogleLatitude::setCurrentLocation(double lat, double lon, double acc) {
    qDebug() << "* GoogleLatitude::setCurrentLocation" << lat << lon << acc;
    CurrentLatitude = lat;
    CurrentLongitude = lon;
    CurrentAccuracy = acc;
}

QUrl GoogleLatitude::getUserAuthorization() {
    qDebug() << "* GoogleLatitude::getUserAuthorization";
    return UserAuthorization;
}

void GoogleLatitude::setAutoConnect(bool status) {
    qDebug() << "* GoogleLatitude::setAutoConnect" << status;
    OauthSettings.setValue("net_auto", status);
}

bool GoogleLatitude::getAutoConnect() {
    qDebug() << "* GoogleLatitude::getAutoConnect";
    return OauthSettings.value("net_auto", true).toBool();
}

void GoogleLatitude::setDaemonMode(bool status) {
    qDebug() << "* GoogleLatitude::setDaemonMode" << status;
    OauthSettings.setValue("daemon", status);
}

bool GoogleLatitude::getDaemonMode() {
    qDebug() << "* GoogleLatitude::getDaemonMode";
    return OauthSettings.value("daemon", true).toBool();
}

void GoogleLatitude::onTemporaryTokenReceived(QString temporaryToken, QString temporaryTokenSecret) {
    qDebug() << "* GoogleLatitude::onTemporaryTokenReceived" << temporaryToken << temporaryTokenSecret;
    if( OauthManager->lastError() == KQOAuthManager::NoError) {
        UserAuthorization = OauthManager->getUserAuthorization(OAuthAuthorizeToken);
        qDebug() << "* GoogleLatitude::onTemporaryTokenReceived" << "UserAuthorization" << UserAuthorization;
        emit needAuth();
    }
}

void GoogleLatitude::onAuthorizationReceived(QString token, QString verifier) {
    qDebug() << "* GoogleLatitude::onAuthorizationReceived" << token << verifier;
    OauthManager->getUserAccessTokens(OAuthGetAccessToken);
    if(OauthManager->lastError() != KQOAuthManager::NoError) {
        emit notToken();
    }
}

void GoogleLatitude::onAccessTokenReceived(QString token, QString tokenSecret) {
    qDebug() << "* GoogleLatitude::onAccessTokenReceived" << token << tokenSecret;
    OauthSettings.setValue("oauth_token", token);
    OauthSettings.setValue("oauth_token_secret", tokenSecret);
    emit gotToken();
}

void GoogleLatitude::onRequestReady(QByteArray response) {
    qDebug() << "* GoogleLatitude::onRequestReady" << response;
    if (response.contains("Invalid Credentials") ) {
        qDebug() << "* GoogleLatitude::onRequestReady" << "Invalid Credentials";
        OauthSettings.remove("oauth_token");
        OauthSettings.remove("oauth_token_secret");
        UserAuthorization.clear();
        emit notToken();
    }
}
