import QtQuick 1.1
import com.nokia.meego 1.0
import QtWebKit 1.0

Page {
    id: mainPage
    orientationLock: PageOrientation.LockPortrait
    Column {
        spacing: 16
        Row {
//            width: rootWindow.width
            Button {
                id: do_staff
                width: rootWindow.width / 3
                text: "..."
                enabled: false
                onClicked: {
                    if (text.match("Start")) {
                        console.log("match start")
                        if (!demonio.demonio_start()) {
                            gps.startUpdates()
                        }
                        text = "Stop"
                    } else if (text.match("Stop")) {
                        console.log("match stop")
                        if (!demonio.demonio_stop()) {
                            gps.stopUpdates(true)
                        }
                        text = "Start"
                    }
                }
            }
        }
        Row {
            Label {
                id: method_label
                text: "Method"
                width: mainPage.width / 4
                anchors.verticalCenter: method_button.verticalCenter
            }
            ButtonColumn {
                id: method_button
                width: mainPage.width * 1 / 3
                Button {
                    id: method_cell
                    text: "Cell Tower"
                    checked: gps.getPositioningMethod() == "cell" ? true : false
                    onClicked: gps.setPositioningMethod("cell")
                }
                Button {
                    id: method_all
                    text: "Both"
                    checked: gps.getPositioningMethod() == "all" ? true : false
                    onClicked: gps.setPositioningMethod("all")
                }
                Button {
                    id: method_agps
                    text: "Only GPS"
                    checked: gps.getPositioningMethod() == "gps" ? true : false
                    onClicked: gps.setPositioningMethod("gps")
                }
            }
        }
        Row {
            Label {
                id: timeout_label
                text: "Time Out"
                width: rootWindow.width / 4
                anchors.verticalCenter: timeout_slider.verticalCenter
            }
            Slider {
                id: timeout_slider
                width: rootWindow.width / 4
                valueIndicatorVisible: true
                minimumValue: 5
                maximumValue: 120
                stepSize: 5
                value: gps.getTimeOut()
            }
            Label {
                id: timeout_value
                text: timeout_slider.value + " seg."
                width: rootWindow.width / 4
                anchors.verticalCenter: timeout_slider.verticalCenter
            }
            Connections {
                target: timeout_slider
                onValueChanged: {
                    timeout_value.text = timeout_slider.value + " seg."
                    gps.setTimeOut(timeout_slider.value)
                }
            }
        }
        Row {
            Label {
                id: interval_label
                text: "Interval"
                width: rootWindow.width / 4
                anchors.verticalCenter: interval_slider.verticalCenter
            }
            Slider {
                id: interval_slider
                width: rootWindow.width / 4
                valueIndicatorVisible: true
                minimumValue: 5
                maximumValue: 60
                stepSize: 5
                value: gps.getInterval() / 60
            }
            Label {
                id: interval_value
                text:  interval_slider.value + " min."
                width: rootWindow.width / 4
                anchors.verticalCenter: interval_slider.verticalCenter
            }
            Connections {
                target: interval_slider
                onValueChanged: {
                    interval_value.text = interval_slider.value + " min."
                    gps.setInterval(interval_slider.value*60)
                }
            }
        }
        Row {
            Label {
                id: connect_label
                text: "Auto Connect"
                width: rootWindow.width / 4
                anchors.verticalCenter: connect_switch.verticalCenter
            }
            Switch {
                id: connect_switch
                width: rootWindow.width / 4
                checked: latitude.getAutoConnect() ? true : false
                onCheckedChanged: {
                    connect_value.text = checked
                    latitude.setAutoConnect(checked)
                }
            }
            Label {
                id: connect_value
                text:  connect_switch.checked
                width: rootWindow.width / 4
                anchors.verticalCenter: connect_switch.verticalCenter
            }
        }
        Row {
            Label {
                id: daemon_label
                text: "Daemon at boot time"
                width: rootWindow.width / 4
                anchors.verticalCenter: daemon_switch.verticalCenter
            }
            Switch {
                id: daemon_switch
                width: rootWindow.width / 4
                checked: latitude.getDaemonMode() ? true : false
                onCheckedChanged: {
                    daemon_value.text = checked
                    latitude.setDaemonMode(checked)                }
            }
            Label {
                id: daemon_value
                text:  daemon_switch.checked
                width: rootWindow.width / 4
                anchors.verticalCenter: daemon_switch.verticalCenter
            }
        }

        Connections {
            target: latitude
            onGotToken: {
                if (demonio.demonio_status()) {
                    do_staff.text = "Stop"
                    do_staff.enabled = true;
                } else {
                    do_staff.text = "Start"
                    do_staff.enabled = true;
                }
                pageStack.pop(login)
            }
        }

        Connections {
            target: latitude
            onNotToken: {
                do_staff.text = "..."
                do_staff.enabled = false;
                if (!demonio.demonio_stop()) {
                    gps.stopUpdates(true)
                }
                latitude.getAccess()
            }
        }
        Connections {
            target: latitude
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
        }
    }

    Component {
        id: login
        Page {
            orientationLock: PageOrientation.LockPortrait
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
                        console.log(url)
                        flickable.contentY = 0;
                        if (url.toString().match("ServiceLogin")) {
                            console.log("* QML WebView go w/2,0")
                            flickable.contentX = width/2;
                        } else {
                            console.log("* QML WebView go 0,0")
                            flickable.contentX = 0;
                        }
                    }
                }
            }
        }
    }
}
