#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include <QtGui/QInputContext>
#include <QtDeclarative/QDeclarativeContext>
#include "gpscontrol.h"
#include "googlelatitude.h"

class EventFilter : public QObject {
protected:
    bool eventFilter(QObject *obj, QEvent *event) {
        QInputContext *ic = qApp->inputContext();
        if (ic) {
            if (ic->focusWidget() == 0 && prevFocusWidget) {
                QEvent closeSIPEvent(QEvent::CloseSoftwareInputPanel);
                ic->filterEvent(&closeSIPEvent);
            } else if (prevFocusWidget == 0 && ic->focusWidget()) {
                QEvent openSIPEvent(QEvent::RequestSoftwareInputPanel);
                ic->filterEvent(&openSIPEvent);
            }
            prevFocusWidget = ic->focusWidget();
        }
        return QObject::eventFilter(obj,event);
    }

private:
    QWidget *prevFocusWidget;
};

Q_DECL_EXPORT int main(int argc, char *argv[]) {
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QScopedPointer<QmlApplicationViewer> viewer(QmlApplicationViewer::create());

    QCoreApplication::setOrganizationName("linfati.com");
    QCoreApplication::setOrganizationDomain("linfati.com");
    QCoreApplication::setApplicationName("latitudeupdater");

    viewer->installEventFilter(new EventFilter);
    viewer->rootContext()->setContextProperty("gps", new GpsControl);
    viewer->rootContext()->setContextProperty("latitude", new GoogleLatitude);

    viewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer->setMainQmlFile(QLatin1String("qml/latitudeupdater/main.qml"));
    viewer->showExpanded();

    qobject_cast<GoogleLatitude *>(
                qvariant_cast<QObject *>(
                    viewer->rootContext()->contextProperty("latitude")))->getAccess();

    return app->exec();
}
