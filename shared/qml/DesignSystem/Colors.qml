// Colors.qml
// Color palette for light and dark themes
pragma Singleton
import QtQuick 2.15

QtObject {
    id: colors

    // Theme state
    property bool darkMode: true

    // Primary palette
    readonly property color primary: darkMode ? "#60A5FA" : "#2563EB"
    readonly property color primaryLight: darkMode ? "#93C5FD" : "#60A5FA"
    readonly property color primaryDark: darkMode ? "#3B82F6" : "#1D4ED8"

    // Secondary palette
    readonly property color secondary: darkMode ? "#A78BFA" : "#7C3AED"
    readonly property color secondaryLight: darkMode ? "#C4B5FD" : "#A78BFA"
    readonly property color secondaryDark: darkMode ? "#8B5CF6" : "#6D28D9"

    // Neutral palette
    readonly property color background: darkMode ? "#0F0F0F" : "#FFFFFF"
    readonly property color surface: darkMode ? "#1A1A1A" : "#F8FAFC"
    readonly property color surfaceElevated: darkMode ? "#262626" : "#FFFFFF"
    readonly property color border: darkMode ? "#333333" : "#E2E8F0"

    // Text colors
    readonly property color textPrimary: darkMode ? "#FFFFFF" : "#0F172A"
    readonly property color textSecondary: darkMode ? "#A3A3A3" : "#64748B"
    readonly property color textTertiary: darkMode ? "#737373" : "#94A3B8"
    readonly property color textInverse: darkMode ? "#0F0F0F" : "#FFFFFF"

    // Semantic colors - Status
    readonly property color success: "#22C55E"
    readonly property color successLight: "#4ADE80"
    readonly property color successDark: "#16A34A"

    readonly property color warning: "#F59E0B"
    readonly property color warningLight: "#FBBF24"
    readonly property color warningDark: "#D97706"

    readonly property color error: "#EF4444"
    readonly property color errorLight: "#F87171"
    readonly property color errorDark: "#DC2626"

    readonly property color info: "#3B82F6"
    readonly property color infoLight: "#60A5FA"
    readonly property color infoDark: "#2563EB"

    // Telltale colors (automotive standard)
    readonly property color telltaleRed: "#FF0000"      // Critical warning
    readonly property color telltaleAmber: "#FFB800"    // Caution
    readonly property color telltaleGreen: "#00FF00"    // Active/OK
    readonly property color telltaleBlue: "#0066FF"     // Informational (high beam)
    readonly property color telltaleWhite: "#FFFFFF"    // General indicator

    // Cluster-specific
    readonly property color clusterBackground: "#000000"
    readonly property color clusterGaugeTrack: "#2A2A2A"
    readonly property color clusterGaugeActive: "#60A5FA"
    readonly property color clusterGaugeWarning: "#F59E0B"
    readonly property color clusterGaugeCritical: "#EF4444"
    readonly property color clusterSpeedText: "#FFFFFF"
    readonly property color clusterUnitText: "#808080"

    // Degraded mode colors
    readonly property color degradedBackground: "#1A0000"
    readonly property color degradedBorder: "#FF4444"
    readonly property color degradedText: "#FFFFFF"

    // Gradients
    readonly property var gradientPrimary: Gradient {
        GradientStop { position: 0.0; color: colors.primary }
        GradientStop { position: 1.0; color: colors.primaryDark }
    }

    readonly property var gradientGauge: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: colors.clusterGaugeActive }
        GradientStop { position: 0.7; color: colors.clusterGaugeWarning }
        GradientStop { position: 1.0; color: colors.clusterGaugeCritical }
    }

    // Helper function to get alpha version of color
    function withAlpha(baseColor, alpha) {
        return Qt.rgba(baseColor.r, baseColor.g, baseColor.b, alpha)
    }

    // Telltale color by priority
    function telltaleColor(priority) {
        switch(priority) {
            case 0: return telltaleRed      // P0 - Critical
            case 1: return telltaleAmber    // P1 - Warning
            case 2: return telltaleGreen    // P2 - Info
            case 3: return telltaleBlue     // P3 - Status
            default: return telltaleWhite
        }
    }
}
