// ClusterView.qml
// Main cluster display layout
// Safety: Display-only, data from validated C++ model

import QtQuick 2.15
import QtQuick.Layouts 1.15
import "adas"

Item {
    id: root

    // Enable keyboard focus for mock ADAS input
    focus: true

    // Mock ADAS state (cycles through states on spacebar)
    property int mockAdasState: 0  // 0=Off, 1=Available, 2=Engaged, 3=Takeover
    property bool useMockData: true

    // Weather simulation
    property int weatherCondition: 0  // 0=Clear, 1=Mist, 2=Rain, 3=Snow
    property var weatherNames: ["Clear", "Mist", "Rain", "Snow"]
    property var weatherColors: ["#000000", "#1a1a2e", "#0a1628", "#1a1a2e"]
    property var weatherOpacity: [0, 0.3, 0.4, 0.35]

    // Time of day
    property int timeOfDay: 1  // 0=Dawn, 1=Day, 2=Dusk, 3=Night
    property var timeNames: ["Dawn", "Day", "Dusk", "Night"]
    property var timeColors: ["#1a0a2e", "#000000", "#2e1a0a", "#000014"]

    // Derived mock values based on state
    property bool mockEngaged: mockAdasState === 2
    property bool mockAvailable: mockAdasState >= 1
    property bool mockTakeoverActive: mockAdasState === 3

    // Animated mock values for realistic simulation
    property double mockSpeed: 0
    property double mockLeadDistanceValue: 50
    property double mockLateralOffsetValue: 0
    property int mockLaneQualityValue: 3

    // ADAS data
    property int mockHmiState: mockAdasState
    property int mockAutomationLevel: mockEngaged ? 2 : 0
    property bool mockAccActive: mockEngaged
    property bool mockLkaActive: mockEngaged
    property bool mockLcaActive: false
    property double mockSetSpeed: mockEngaged ? 100 : 0
    property double mockFollowingGap: 2.0
    property bool mockHasValidLanes: mockEngaged
    property int mockLaneQuality: mockEngaged ? mockLaneQualityValue : 0
    property bool mockHasLeadVehicle: mockEngaged
    property double mockLeadDistance: mockEngaged ? mockLeadDistanceValue : 0
    property double mockLateralOffset: mockLateralOffsetValue
    property int mockTakeoverUrgency: mockTakeoverActive ? 2 : 0
    property double mockTakeoverCountdown: mockTakeoverActive ? 7 : 0
    property string mockTakeoverMessage: mockTakeoverActive ? "TAKE OVER CONTROL" : ""
    property bool mockMrmActive: false

    // Mock data animation timer
    Timer {
        id: mockDataTimer
        interval: 100  // 10 Hz update
        running: mockEngaged
        repeat: true
        onTriggered: {
            // Simulate speed variation
            mockSpeed = 95 + Math.sin(Date.now() / 2000) * 5

            // Simulate lead vehicle distance variation
            mockLeadDistanceValue = 40 + Math.sin(Date.now() / 3000) * 15

            // Simulate slight lateral wandering
            mockLateralOffsetValue = Math.sin(Date.now() / 4000) * 0.3

            // Occasionally vary lane quality
            if (Math.random() < 0.02) {
                mockLaneQualityValue = 2 + Math.floor(Math.random() * 2)
            }
        }
    }

    // State change handler
    onMockAdasStateChanged: {
        console.log("ADAS State:", ["Off", "Available", "Engaged", "Takeover"][mockAdasState])
        if (mockAdasState === 2) {
            // Reset to good values when engaging
            mockSpeed = 100
            mockLeadDistanceValue = 45
            mockLateralOffsetValue = 0
            mockLaneQualityValue = 3
        }
    }

    // Keyboard handler for spacebar
    Keys.onSpacePressed: {
        mockAdasState = (mockAdasState + 1) % 4
    }

    // Additional key handlers for specific states
    Keys.onPressed: function(event) {
        if (event.key === Qt.Key_1) {
            mockAdasState = 0  // Off
        } else if (event.key === Qt.Key_2) {
            mockAdasState = 1  // Available
        } else if (event.key === Qt.Key_3) {
            mockAdasState = 2  // Engaged
        } else if (event.key === Qt.Key_4) {
            mockAdasState = 3  // Takeover
        } else if (event.key === Qt.Key_M) {
            mockMrmActive = !mockMrmActive
            if (mockMrmActive) mockAdasState = 3
            console.log("MRM Active:", mockMrmActive)
        } else if (event.key === Qt.Key_L) {
            // Cycle lane quality
            mockLaneQualityValue = (mockLaneQualityValue % 3) + 1
            console.log("Lane Quality:", mockLaneQualityValue)
        } else if (event.key === Qt.Key_D) {
            // Toggle lead vehicle distance (near/far)
            mockLeadDistanceValue = mockLeadDistanceValue > 30 ? 15 : 50
            console.log("Lead Distance:", mockLeadDistanceValue)
        } else if (event.key === Qt.Key_W) {
            // Cycle weather conditions
            weatherCondition = (weatherCondition + 1) % 4
            console.log("Weather:", weatherNames[weatherCondition])
        } else if (event.key === Qt.Key_T) {
            // Cycle time of day
            timeOfDay = (timeOfDay + 1) % 4
            console.log("Time of Day:", timeNames[timeOfDay])
        }
    }

    // Background with time of day
    Rectangle {
        anchors.fill: parent
        color: clusterViewModel.isDegraded ? "#1A0000" : timeColors[timeOfDay]

        Behavior on color {
            ColorAnimation {
                duration: 500
            }
        }
    }

    // Weather overlay
    Rectangle {
        id: weatherOverlay
        anchors.fill: parent
        color: weatherColors[weatherCondition]
        opacity: weatherOpacity[weatherCondition]
        z: 50

        Behavior on opacity {
            NumberAnimation { duration: 500 }
        }

        Behavior on color {
            ColorAnimation { duration: 500 }
        }
    }

    // Rain effect
    Item {
        anchors.fill: parent
        visible: weatherCondition === 2
        z: 51

        Repeater {
            model: 50
            Rectangle {
                property real startX: Math.random() * parent.width
                property real startY: Math.random() * parent.height
                property real speed: 3 + Math.random() * 4

                x: startX
                y: startY
                width: 2
                height: 15 + Math.random() * 10
                radius: 1
                color: "#4080B0"
                opacity: 0.4 + Math.random() * 0.3

                NumberAnimation on y {
                    from: -30
                    to: parent.parent.height + 30
                    duration: 1000 / speed * 100
                    loops: Animation.Infinite
                }
            }
        }
    }

    // Snow effect
    Item {
        anchors.fill: parent
        visible: weatherCondition === 3
        z: 51

        Repeater {
            model: 40
            Rectangle {
                property real startX: Math.random() * parent.width
                property real startY: Math.random() * parent.height
                property real speed: 1 + Math.random() * 2
                property real drift: Math.random() * 2 - 1

                x: startX
                y: startY
                width: 4 + Math.random() * 4
                height: width
                radius: width / 2
                color: "#FFFFFF"
                opacity: 0.5 + Math.random() * 0.4

                NumberAnimation on y {
                    from: -20
                    to: parent.parent.height + 20
                    duration: 3000 / speed * 100
                    loops: Animation.Infinite
                }

                NumberAnimation on x {
                    from: startX - 30
                    to: startX + 30
                    duration: 2000
                    loops: Animation.Infinite
                    easing.type: Easing.InOutSine
                }
            }
        }
    }

    // Mist effect
    Rectangle {
        anchors.fill: parent
        visible: weatherCondition === 1
        z: 51
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#00808080" }
            GradientStop { position: 0.5; color: "#40808080" }
            GradientStop { position: 1.0; color: "#60808080" }
        }
        opacity: 0.5

        // Animate mist
        NumberAnimation on opacity {
            from: 0.4
            to: 0.6
            duration: 3000
            loops: Animation.Infinite
            easing.type: Easing.InOutSine
        }
    }

    // Degraded mode view
    DegradedModeView {
        anchors.fill: parent
        visible: clusterViewModel.isDegraded
        z: 100
    }

    // Normal content
    Item {
        anchors.fill: parent
        opacity: clusterViewModel.isDegraded ? 0.5 : 1.0
        visible: !clusterViewModel.isDegraded || true  // Always visible in degraded

        Behavior on opacity {
            NumberAnimation {
                duration: 300
            }
        }

        // Left section - Secondary gauges
        Column {
            anchors.left: parent.left
            anchors.leftMargin: 48
            anchors.verticalCenter: parent.verticalCenter
            spacing: 32
            visible: clusterViewModel.showSecondaryGauges

            // Battery gauge
            Item {
                width: 160
                height: 160

                Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: "transparent"
                    border.color: "#333333"
                    border.width: 8
                }

                Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: "transparent"
                    border.color: getBatteryColor()
                    border.width: 8

                    function getBatteryColor() {
                        var level = clusterViewModel.batteryLevel;
                        if (level < 20)
                            return "#EF4444";
                        if (level < 40)
                            return "#F59E0B";
                        return "#22C55E";
                    }
                }

                Column {
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: clusterViewModel.batteryValid ? Math.round(clusterViewModel.batteryLevel) + "%" : "—"
                        font.pixelSize: 32
                        font.weight: Font.Medium
                        color: "#FFFFFF"
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "BATTERY"
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: "#808080"
                    }
                }
            }

            // Range display (dual units)
            Column {
                width: 160
                spacing: 2

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: clusterViewModel.batteryValid ? Math.round(clusterViewModel.range) : "—"
                    font.pixelSize: 40
                    font.weight: Font.Light
                    color: "#FFFFFF"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "km RANGE"
                    font.pixelSize: 12
                    font.weight: Font.Medium
                    color: "#FFFFFF"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: clusterViewModel.batteryValid ? Math.round(clusterViewModel.range * 0.621371) + " mi" : "—"
                    font.pixelSize: 14
                    color: "#808080"
                }
            }
        }

        // Center - Drive View (Speed + Gear)
        DriveView {
            anchors.centerIn: parent
        }

        // Right section - Info panel
        Column {
            anchors.right: parent.right
            anchors.rightMargin: 48
            anchors.verticalCenter: parent.verticalCenter
            spacing: 24
            visible: clusterViewModel.showInfoPanel

            // Time
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: clusterViewModel.timeDisplay
                font.pixelSize: 48
                font.weight: Font.Light
                color: "#FFFFFF"
            }

            // Outside temperature (dual units)
            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 2

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Math.round(clusterViewModel.outsideTemp) + "°C"
                    font.pixelSize: 24
                    color: "#FFFFFF"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Math.round(clusterViewModel.outsideTemp * 9 / 5 + 32) + "°F"
                    font.pixelSize: 16
                    color: "#808080"
                }
            }

            // Power consumption
            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 4

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Math.round(clusterViewModel.power) + " kW"
                    font.pixelSize: 32
                    font.weight: Font.Medium
                    color: clusterViewModel.power > 0 ? "#60A5FA" : "#22C55E"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: clusterViewModel.power > 0 ? "CONSUMING" : "REGEN"
                    font.pixelSize: 12
                    color: "#808080"
                }
            }
        }

        // Top bar - Telltales
        TelltaleBar {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 24
            height: 48
        }

        // Large left turn indicator (bottom left)
        TurnIndicator {
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.leftMargin: 48
            anchors.bottomMargin: 100
            width: 160
            height: 120
            isLeft: true
            active: leftSideTurnActive
            activeColor: "#22C55E"
            opacity: leftSideTurnActive ? 1.0 : 0.0
            scale: leftSideTurnActive ? 1.0 : 0.8

            Behavior on opacity {
                NumberAnimation {
                    duration: 200
                }
            }

            Behavior on scale {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutBack
                }
            }

            property bool leftSideTurnActive: {
                var telltales = clusterViewModel.activeTelltales;
                for (var i = 0; i < telltales.length; i++) {
                    if (telltales[i].id.indexOf("turn_left") >= 0) {
                        return telltales[i].active;
                    }
                }
                return false;
            }
        }

        // Large right turn indicator (bottom right)
        TurnIndicator {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: 48
            anchors.bottomMargin: 100
            width: 160
            height: 120
            isLeft: false
            active: rightSideTurnActive
            activeColor: "#22C55E"
            opacity: rightSideTurnActive ? 1.0 : 0.0
            scale: rightSideTurnActive ? 1.0 : 0.8

            Behavior on opacity {
                NumberAnimation {
                    duration: 200
                }
            }

            Behavior on scale {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutBack
                }
            }

            property bool rightSideTurnActive: {
                var telltales = clusterViewModel.activeTelltales;
                for (var i = 0; i < telltales.length; i++) {
                    if (telltales[i].id.indexOf("turn_right") >= 0) {
                        return telltales[i].active;
                    }
                }
                return false;
            }
        }

        // Alert overlay
        AlertOverlay {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 24
        }

        // ADAS State Zone - shows engagement status
        AdasStateZone {
            id: adasStateZone
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 24
            width: 400
            height: 120

            // Use mock data
            hmiState: root.mockHmiState
            hmiStateText: ["Off", "Available", "Engaged", "Takeover"][root.mockAdasState]
            automationLevel: root.mockAutomationLevel
            engaged: root.mockEngaged
            available: root.mockAvailable
            accActive: root.mockAccActive
            lkaActive: root.mockLkaActive
            lcaActive: root.mockLcaActive
            setSpeed: root.mockSetSpeed
            followingGap: root.mockFollowingGap
        }

        // Perception Mini View - shows lane and lead vehicle
        PerceptionMiniView {
            id: perceptionMiniView
            anchors.right: parent.right
            anchors.bottom: adasStateZone.top
            anchors.rightMargin: 48
            anchors.bottomMargin: 16
            width: 180
            height: 200
            visible: root.mockEngaged

            hasValidLanes: root.mockHasValidLanes
            laneQuality: root.mockLaneQuality
            hasLeadVehicle: root.mockHasLeadVehicle
            leadDistance: root.mockLeadDistance
            lateralOffset: root.mockLateralOffset
        }
    }

    // Degraded mode border
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: "#FF4444"
        border.width: clusterViewModel.isDegraded ? 4 : 0
        visible: clusterViewModel.isDegraded

        SequentialAnimation on border.width {
            running: clusterViewModel.isDegraded
            loops: Animation.Infinite
            NumberAnimation {
                to: 2
                duration: 500
            }
            NumberAnimation {
                to: 4
                duration: 500
            }
        }
    }

    // ADAS Takeover Banner - z-index 1000, cannot be occluded (SR-CL-ADAS-120)
    TakeoverBanner {
        id: takeoverBanner
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        z: 1000  // Highest z-index - cannot be occluded

        active: root.mockTakeoverActive
        urgency: root.mockTakeoverUrgency
        countdown: root.mockTakeoverCountdown
        message: root.mockTakeoverMessage
        mrmActive: root.mockMrmActive
    }

    // Key hint overlay
    Rectangle {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 8
        width: statusColumn.width + 16
        height: statusColumn.height + 12
        radius: 4
        color: "#000000C0"
        z: 200

        Column {
            id: statusColumn
            anchors.centerIn: parent
            spacing: 4

            // ADAS state indicator
            Row {
                spacing: 6
                Rectangle {
                    width: 8
                    height: 8
                    radius: 4
                    anchors.verticalCenter: parent.verticalCenter
                    color: root.mockEngaged ? "#22C55E" : (root.mockAvailable ? "#60A5FA" : "#808080")
                }
                Text {
                    text: "ADAS: " + ["Off", "Available", "Engaged", "Takeover"][root.mockAdasState]
                    font.pixelSize: 10
                    color: root.mockEngaged ? "#22C55E" : (root.mockAvailable ? "#60A5FA" : "#808080")
                }
            }

            // Weather and time status
            Row {
                spacing: 12
                Text {
                    text: "Weather: " + weatherNames[weatherCondition]
                    font.pixelSize: 10
                    color: weatherCondition === 0 ? "#808080" : "#60A5FA"
                }
                Text {
                    text: "Time: " + timeNames[timeOfDay]
                    font.pixelSize: 10
                    color: "#F59E0B"
                }
            }

            // Key hints
            Text {
                text: "SPACE: Cycle | 1-4: States | M: MRM"
                font.pixelSize: 9
                color: "#808080"
            }
            Text {
                text: "W: Weather | T: Time | L: Lane | D: Distance"
                font.pixelSize: 9
                color: "#808080"
            }
        }
    }
}
