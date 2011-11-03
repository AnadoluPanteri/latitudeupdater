import QtQuick 1.1
import com.nokia.meego 1.0
import QtWebKit 1.0

Page {
    orientationLock: PageOrientation.LockPortrait
    anchors.margins: 10
    Column {
        anchors.fill: parent
        spacing: 16
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Label {
                text: qsTr("Latitude Updater")
                scale: 2
            }
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Button {
                id: do_staff
                text: qsTr("...")
                enabled: false
                onClicked: {
                    if (text.match(qsTr("Start"))) {
                        gps.startUpdates()
                        text = qsTr("Stop")
                    } else if (text.match(qsTr("Stop"))) {
                        gps.stopUpdates(true)
                        text = qsTr("Start")
                    }
                }
            }
        }
        Row {
            Label {
                text: qsTr("Method")
                anchors.verticalCenter: method_button.verticalCenter
                width: parent.parent.width / 4
            }
            ButtonColumn {
                id: method_button
                Button {
                    text: qsTr("Cell Tower")
                    checked: gps.getPositioningMethod() == "cell" ? true : false
                    onClicked: gps.setPositioningMethod("cell")
                }
                Button {
                    text: qsTr("Both")
                    checked: gps.getPositioningMethod() == "all" ? true : false
                    onClicked: gps.setPositioningMethod("all")
                }
                Button {
                    text: qsTr("Only GPS")
                    checked: gps.getPositioningMethod() == "gps" ? true : false
                    onClicked: gps.setPositioningMethod("gps")
                }
            }
        }
        Row {
            Label {
                text: qsTr("Time Out")
                anchors.verticalCenter: timeout_slider.verticalCenter
                width: parent.parent.width / 4
            }
            Slider {
                id: timeout_slider
                width: parent.parent.width * 3 / 4
                valueIndicatorVisible: true
                minimumValue: 5
                maximumValue: 120
                stepSize: 5
                value: gps.getTimeOut()
                function formatValue(v) { return v + qsTr(" sec."); }
            }
            Connections {
                target: timeout_slider
                onValueChanged: gps.setTimeOut(timeout_slider.value)
            }
        }
        Row {
            Label {
                text: qsTr("Interval")
                anchors.verticalCenter: interval_slider.verticalCenter
                width: parent.parent.width / 4
            }
            Slider {
                id: interval_slider
                width: parent.parent.width * 3 / 4
                valueIndicatorVisible: true
                minimumValue: 5
                maximumValue: 60
                stepSize: 5
                value: gps.getInterval() / 60
                function formatValue(v) { return v + qsTr(" min."); }
            }
            Connections {
                target: interval_slider
                onValueChanged: gps.setInterval(interval_slider.value*60)
            }
        }
        Row {
            Label {
                text: qsTr("Auto Connect")
                anchors.verticalCenter: connect_switch.verticalCenter
                width: parent.parent.width / 3
            }
            Switch {
                id: connect_switch
                checked: latitude.getAutoConnect() ? true : false
                onCheckedChanged: latitude.setAutoConnect(checked)
            }
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Label {
                text: qsTr("Last five Position")
            }
        }
        Row {
            Label {
                id: gps_string1
                text: qsTr("...")
            }
        }
        Row {
            Label {
                id: gps_string2
                text: qsTr("...")
            }
        }
        Row {
            Label {
                id: gps_string3
                text: qsTr("...")
            }
        }
        Row {
            Label {
                id: gps_string4
                text: qsTr("...")
            }
        }
        Row {
            Label {
                id: gps_string5
                text: qsTr("...")
            }
        }

        Connections {
            target: latitude
            onGotToken: {
                do_staff.text = qsTr("Start")
                do_staff.enabled = true;
                pageStack.pop(login)
            }
            onNotToken: {
                do_staff.text = qsTr("...")
                do_staff.enabled = false;
                gps.stopUpdates(true)
                latitude.getAccess()
            }
            onNeedAuth: {
                pageStack.push(login)
            }
        }

        Connections {
            target: gps
            onGotFix: {
                latitude.setCurrentLocation(gps.getCurrentLatitude(),
                                            gps.getCurrentLongitude(),
                                            gps.getCurrentAccuracy())
                latitude.sendCurrentLocation()
            }
            onGotUpdate: {
                gps_string5.text = gps_string4.text
                gps_string4.text = gps_string3.text
                gps_string3.text = gps_string2.text
                gps_string2.text = gps_string1.text
                gps_string1.text =  gps.getCount() + qsTr(": ") +
                        qsTr("Lat: ") + gps.getCurrentLatitude().toFixed(5) +
                        qsTr(" Lon: ") + gps.getCurrentLongitude().toFixed( 5) +
                        qsTr(" Acc: ") + gps.getCurrentAccuracy().toFixed(1)
            }
        }
        Component.onCompleted: {
            latitude.getAccess()
        }
    }


    Component {
        id: login
        Page {
            orientationLock: PageOrientation.LockPrevious
            Flickable {
                id: flickable
                width: parent.width
                height: parent.height
                contentWidth: webView.width
                contentHeight: webView.height
                WebView {
                    id: webView
                    url: latitude.getUserAuthorization()
                    preferredWidth: parent.width
                    preferredHeight: parent.height
                    onLoadFinished: {
                        flickable.contentX = 0;
                        flickable.contentY = 0;
                        if (url.toString().match("ServiceLogin")) {
                            flickable.contentX = width/2;
                        }
                    }
                }
            }
        }
    }
}
