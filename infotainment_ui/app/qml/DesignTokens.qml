// DesignTokens.qml
// Centralized design system tokens for consistent UI

pragma Singleton
import QtQuick 2.15

QtObject {
    // Color Palette
    readonly property color colorBackground: "#03090F"
    readonly property color colorBackgroundElevated: "#0B1118"
    readonly property color colorBackgroundCard: "#111821"
    readonly property color colorBackgroundHover: "#1A2229"
    readonly property color colorBackgroundActive: "#1F3442"

    readonly property color colorSurface: "#0F0F0F"
    readonly property color colorSurfaceElevated: "#1A1A1A"
    readonly property color colorSurfaceHover: "#262626"

    readonly property color colorBorder: "#1C262F"
    readonly property color colorBorderLight: "#20303D"
    readonly property color colorBorderActive: "#4DC9CB"

    readonly property color colorTextPrimary: "#F4F7FB"
    readonly property color colorTextSecondary: "#8AA3B8"
    readonly property color colorTextTertiary: "#7F95A6"
    readonly property color colorTextMuted: "#6F8797"

    readonly property color colorAccentPrimary: "#4DC9CB"
    readonly property color colorAccentBlue: "#60A5FA"
    readonly property color colorAccentGreen: "#22C55E"
    readonly property color colorAccentPurple: "#A78BFA"
    readonly property color colorAccentPink: "#F472B6"
    readonly property color colorAccentOrange: "#F59E0B"
    readonly property color colorAccentRed: "#EF4444"
    readonly property color colorAccentCyan: "#06B6D4"

    // Spacing
    readonly property int spacing4: 4
    readonly property int spacing8: 8
    readonly property int spacing12: 12
    readonly property int spacing16: 16
    readonly property int spacing20: 20
    readonly property int spacing24: 24
    readonly property int spacing32: 32
    readonly property int spacing48: 48

    // Border Radius
    readonly property int radiusSmall: 8
    readonly property int radiusMedium: 12
    readonly property int radiusLarge: 16
    readonly property int radiusXLarge: 20
    readonly property int radiusXXLarge: 24
    readonly property int radiusRound: 32

    // Typography
    readonly property int fontSizeXSmall: 11
    readonly property int fontSizeSmall: 12
    readonly property int fontSizeMedium: 14
    readonly property int fontSizeBase: 16
    readonly property int fontSizeLarge: 18
    readonly property int fontSizeXLarge: 20
    readonly property int fontSizeXXLarge: 24
    readonly property int fontSizeHuge: 34

    // Shadows
    readonly property string shadowSmall: "#00000040"
    readonly property string shadowMedium: "#00000060"
    readonly property string shadowLarge: "#00000080"
    readonly property string shadowXLarge: "#00000090"

    // Animation Durations
    readonly property int durationFast: 150
    readonly property int durationNormal: 250
    readonly property int durationSlow: 350

    // Transitions
    readonly property int transitionQuick: 200
    readonly property int transitionSmooth: 300
}
