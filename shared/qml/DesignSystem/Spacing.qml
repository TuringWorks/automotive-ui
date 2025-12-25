// Spacing.qml
// Spacing scale based on 4pt grid
pragma Singleton
import QtQuick 2.15

QtObject {
    id: spacing

    // Base unit (4pt grid)
    readonly property int base: 4

    // Spacing scale
    readonly property int xxs: 2    // 0.5x
    readonly property int xs: 4     // 1x
    readonly property int sm: 8     // 2x
    readonly property int md: 12    // 3x
    readonly property int lg: 16    // 4x
    readonly property int xl: 24    // 6x
    readonly property int xxl: 32   // 8x
    readonly property int xxxl: 48  // 12x

    // Component spacing
    readonly property int componentPadding: lg
    readonly property int cardPadding: xl
    readonly property int listItemPadding: lg
    readonly property int iconMargin: sm
    readonly property int sectionGap: xxl

    // Layout margins
    readonly property int screenMarginMobile: lg
    readonly property int screenMarginTablet: xl
    readonly property int screenMarginDesktop: xxl
    readonly property int screenMarginCluster: xl

    // Grid
    readonly property int gridGutter: lg
    readonly property int gridColumnCount: 12

    // Cluster-specific spacing
    readonly property int clusterPadding: xl
    readonly property int gaugeSpacing: xl
    readonly property int telltaleSpacing: sm
    readonly property int alertPadding: lg

    // Infotainment-specific
    readonly property int navBarHeight: 64
    readonly property int bottomBarHeight: 72
    readonly property int drawerWidth: 280
    readonly property int cardGap: lg

    // Helper function for custom multiples
    function units(n) {
        return base * n
    }
}
