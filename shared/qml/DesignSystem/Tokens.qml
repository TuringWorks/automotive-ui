// Tokens.qml
// Design system tokens - single source of truth for design values
pragma Singleton
import QtQuick 2.15

QtObject {
    id: tokens

    // Animation durations
    readonly property int animationFast: 150
    readonly property int animationNormal: 250
    readonly property int animationSlow: 400

    // Easing curves
    readonly property var easingDefault: Easing.OutCubic
    readonly property var easingEnter: Easing.OutQuart
    readonly property var easingExit: Easing.InCubic

    // Border radii
    readonly property int radiusSmall: 4
    readonly property int radiusMedium: 8
    readonly property int radiusLarge: 16
    readonly property int radiusRound: 9999

    // Shadows (elevation levels)
    readonly property var shadowSmall: ({
        horizontalOffset: 0,
        verticalOffset: 1,
        radius: 3,
        color: "#26000000"
    })
    readonly property var shadowMedium: ({
        horizontalOffset: 0,
        verticalOffset: 2,
        radius: 8,
        color: "#33000000"
    })
    readonly property var shadowLarge: ({
        horizontalOffset: 0,
        verticalOffset: 4,
        radius: 16,
        color: "#40000000"
    })

    // Z-index layers
    readonly property int zBackground: 0
    readonly property int zContent: 10
    readonly property int zOverlay: 100
    readonly property int zPopup: 200
    readonly property int zAlert: 300
    readonly property int zCriticalAlert: 400

    // Opacity levels
    readonly property real opacityDisabled: 0.38
    readonly property real opacityMedium: 0.60
    readonly property real opacityHigh: 0.87
    readonly property real opacityFull: 1.0

    // Cluster-specific
    readonly property int clusterUpdateRateHz: 60
    readonly property int signalUpdateRateHz: 20
    readonly property int staleThresholdMs: 300

    // Touch targets (minimum sizes for touch interaction)
    readonly property int touchTargetMin: 44
    readonly property int touchTargetPreferred: 48

    // Gauge sizes
    readonly property int gaugeSmall: 120
    readonly property int gaugeMedium: 200
    readonly property int gaugeLarge: 300
    readonly property int gaugeXLarge: 400
}
