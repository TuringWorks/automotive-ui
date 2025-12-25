// Animations.qml
// Animation presets and behaviors
pragma Singleton
import QtQuick 2.15

QtObject {
    id: animations

    // Duration presets (in ms)
    readonly property int instant: 0
    readonly property int fast: 100
    readonly property int normal: 200
    readonly property int slow: 300
    readonly property int verySlow: 500

    // Cluster-specific (optimized for glanceability)
    readonly property int gaugeUpdate: 100
    readonly property int telltaleFlash: 500
    readonly property int alertEnter: 200
    readonly property int alertExit: 150
    readonly property int degradedTransition: 300

    // Infotainment
    readonly property int pageTransition: 250
    readonly property int modalEnter: 200
    readonly property int modalExit: 150
    readonly property int listItemEnter: 150
    readonly property int ripple: 400

    // Easing curves
    readonly property int easingTypeDefault: Easing.OutCubic
    readonly property int easingTypeEnter: Easing.OutQuart
    readonly property int easingTypeExit: Easing.InCubic
    readonly property int easingTypeBounce: Easing.OutBack
    readonly property int easingTypeSpring: Easing.OutElastic

    // Behavior for smooth value changes
    function smoothValue(duration) {
        return {
            duration: duration || normal,
            easing: { type: easingTypeDefault }
        }
    }

    // Opacity fade
    function fade(duration, easingType) {
        return {
            duration: duration || normal,
            easing: { type: easingType || easingTypeDefault }
        }
    }

    // Scale animation
    function scale(duration, from, to) {
        return {
            duration: duration || normal,
            from: from || 0.95,
            to: to || 1.0,
            easing: { type: easingTypeEnter }
        }
    }

    // Slide animation
    function slide(duration, direction) {
        var offset = 50
        var props = {
            duration: duration || normal,
            easing: { type: easingTypeDefault }
        }

        switch(direction) {
            case "left":
                props.from = offset
                props.property = "x"
                break
            case "right":
                props.from = -offset
                props.property = "x"
                break
            case "up":
                props.from = offset
                props.property = "y"
                break
            case "down":
                props.from = -offset
                props.property = "y"
                break
        }

        return props
    }
}
