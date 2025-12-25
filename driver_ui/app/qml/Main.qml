// Main.qml
// Driver UI main window
// Safety: Display-only, all logic in C++ safety core

import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: window

    width: 1920
    height: 720
    visible: true
    title: qsTr("Driver UI - Cluster")
    color: "#000000"

    // Cluster view
    ClusterView {
        id: clusterView
        anchors.fill: parent
    }

    // Frame counter for safety monitor
    Timer {
        interval: 16  // ~60fps
        running: true
        repeat: true
        onTriggered: {
            if (typeof clusterApp !== 'undefined' && clusterApp.safetyMonitor) {
                clusterApp.safetyMonitor.recordFrame();
            }
        }
    }

    // Keyboard shortcuts for testing
    Shortcut {
        sequence: "F1"
        onActivated: {
            if (typeof faultInjector !== 'undefined') {
                faultInjector.startScenario(1, 5000);  // Stale speed
            }
        }
    }

    Shortcut {
        sequence: "F2"
        onActivated: {
            if (typeof faultInjector !== 'undefined') {
                faultInjector.startScenario(5, 5000);  // Multiple stale
            }
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: {
            if (typeof faultInjector !== 'undefined') {
                faultInjector.stopScenario();
            }
        }
    }

    // Turn signal testing shortcuts
    Shortcut {
        sequence: "Left"
        onActivated: {
            if (typeof signalHub !== 'undefined') {
                // Toggle left turn signal
                console.log("Left turn signal toggled");
            }
        }
    }

    Shortcut {
        sequence: "Right"
        onActivated: {
            if (typeof signalHub !== 'undefined') {
                // Toggle right turn signal
                console.log("Right turn signal toggled");
            }
        }
    }
}
