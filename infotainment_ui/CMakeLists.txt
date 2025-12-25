# infotainment_ui/CMakeLists.txt
# Infotainment UI - Non-safety but cybersecurity-critical

include(${CMAKE_SOURCE_DIR}/cmake/AutomotiveCompilerSettings.cmake)

# ============================================================================
# Infotainment Core Library
# ============================================================================

add_library(infotainment_core STATIC
    app/cpp/AppFramework.cpp
    app/cpp/PermissionManager.cpp
    app/cpp/SecureSettings.cpp
    app/cpp/MediaController.cpp
    app/cpp/NavigationController.cpp
    app/cpp/PhoneController.cpp
    app/cpp/ProjectionController.cpp
    app/cpp/VehicleSettingsController.cpp
)

target_include_directories(infotainment_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/app/cpp
)

target_link_libraries(infotainment_core PUBLIC
    automotive_platform
    Qt6::Core
    Qt6::Qml
)

automotive_standard_settings(infotainment_core)

# ============================================================================
# Infotainment UI Application
# ============================================================================

qt_add_executable(infotainment_ui
    app/main.cpp
)

qt_add_qml_module(infotainment_ui
    URI InfotainmentUI
    VERSION 1.0
    IMPORTS Qt5Compat.GraphicalEffects
    QML_FILES
        app/qml/Main.qml
        app/qml/AppShell.qml
        app/qml/DesignTokens.qml
        app/qml/HomePage.qml
        app/qml/MediaPage.qml
        app/qml/NavigationPage.qml
        app/qml/PhonePage.qml
        app/qml/ProjectionPage.qml
        app/qml/VehiclePage.qml
        app/qml/SettingsPage.qml
        app/qml/AppDrawer.qml
        app/qml/NavigationRail.qml
        app/qml/NowPlayingBar.qml
    RESOURCES
        app/resources/resources.qrc
)

target_include_directories(infotainment_ui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/app/cpp
)

target_link_libraries(infotainment_ui PRIVATE
    infotainment_core
    automotive_platform
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Positioning
    Qt6::Location
)

# Import QML modules
target_compile_definitions(infotainment_ui PRIVATE
    QML_IMPORT_PATH="${CMAKE_BINARY_DIR}"
)

# macOS bundle settings
set_target_properties(infotainment_ui PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.automotive.infotainmentui"
    MACOSX_BUNDLE_BUNDLE_NAME "Infotainment UI"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    WIN32_EXECUTABLE TRUE
)

# Installation
install(TARGETS infotainment_ui
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
