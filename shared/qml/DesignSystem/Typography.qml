// Typography.qml
// Typography scale and text styles
pragma Singleton
import QtQuick 2.15

QtObject {
    id: typography

    // Font family
    readonly property string fontFamily: "Inter, SF Pro Display, Segoe UI, Roboto, sans-serif"
    readonly property string fontFamilyMono: "JetBrains Mono, SF Mono, Consolas, monospace"

    // Font weights
    readonly property int weightLight: Font.Light
    readonly property int weightRegular: Font.Normal
    readonly property int weightMedium: Font.Medium
    readonly property int weightSemiBold: Font.DemiBold
    readonly property int weightBold: Font.Bold

    // Text sizes (following 8pt grid, major second scale ~1.125)
    readonly property int sizeXs: 10
    readonly property int sizeSm: 12
    readonly property int sizeMd: 14
    readonly property int sizeLg: 16
    readonly property int sizeXl: 20
    readonly property int size2xl: 24
    readonly property int size3xl: 32
    readonly property int size4xl: 40
    readonly property int size5xl: 56
    readonly property int size6xl: 72
    readonly property int size7xl: 96

    // Line heights
    readonly property real lineHeightTight: 1.1
    readonly property real lineHeightNormal: 1.4
    readonly property real lineHeightRelaxed: 1.6

    // Letter spacing
    readonly property real trackingTight: -0.02
    readonly property real trackingNormal: 0
    readonly property real trackingWide: 0.05

    // Cluster-specific sizes
    readonly property int speedPrimary: 96
    readonly property int speedSecondary: 48
    readonly property int gearDisplay: 64
    readonly property int unitLabel: 16
    readonly property int telltaleLabel: 10

    // Infotainment sizes
    readonly property int navTitle: 24
    readonly property int listItem: 16
    readonly property int caption: 12

    // Text style presets
    function display1() {
        return {
            pixelSize: size7xl,
            weight: weightLight,
            letterSpacing: trackingTight,
            lineHeight: lineHeightTight
        }
    }

    function display2() {
        return {
            pixelSize: size5xl,
            weight: weightRegular,
            letterSpacing: trackingTight,
            lineHeight: lineHeightTight
        }
    }

    function headline1() {
        return {
            pixelSize: size3xl,
            weight: weightSemiBold,
            letterSpacing: trackingNormal,
            lineHeight: lineHeightTight
        }
    }

    function headline2() {
        return {
            pixelSize: size2xl,
            weight: weightMedium,
            letterSpacing: trackingNormal,
            lineHeight: lineHeightNormal
        }
    }

    function body1() {
        return {
            pixelSize: sizeLg,
            weight: weightRegular,
            letterSpacing: trackingNormal,
            lineHeight: lineHeightNormal
        }
    }

    function body2() {
        return {
            pixelSize: sizeMd,
            weight: weightRegular,
            letterSpacing: trackingNormal,
            lineHeight: lineHeightNormal
        }
    }

    function caption1() {
        return {
            pixelSize: sizeSm,
            weight: weightMedium,
            letterSpacing: trackingWide,
            lineHeight: lineHeightNormal
        }
    }

    function button() {
        return {
            pixelSize: sizeMd,
            weight: weightSemiBold,
            letterSpacing: trackingWide,
            lineHeight: lineHeightTight
        }
    }
}
