# driver_ui/CMakeLists.txt
# Driver UI (Cluster) - Safety-relevant application

include(${CMAKE_SOURCE_DIR}/cmake/AutomotiveCompilerSettings.cmake)

# ============================================================================
# Safety Core Library
# ============================================================================

add_library(driver_safety_core STATIC
    safety_core/cpp/ClusterStateModel.cpp
    safety_core/cpp/AlertManager.cpp
    safety_core/cpp/TelltaleManager.cpp
    safety_core/cpp/DegradedModeController.cpp
    safety_core/cpp/SafetyMonitor.cpp
    safety_core/cpp/FaultInjector.cpp
)

target_include_directories(driver_safety_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/safety_core/cpp
)

target_link_libraries(driver_safety_core PUBLIC
    automotive_platform
    Qt6::Core
)

# Apply strict safety compiler settings
automotive_safety_settings(driver_safety_core)

# ============================================================================
# Driver UI Application
# ============================================================================

qt_add_executable(driver_ui
    app/main.cpp
    app/cpp/ClusterApplication.cpp
    app/cpp/ClusterViewModel.cpp
    app/cpp/AdasViewModel.cpp
)

qt_add_qml_module(driver_ui
    URI DriverUI
    VERSION 1.0
    QML_FILES
        app/qml/Main.qml
        app/qml/ClusterView.qml
        app/qml/DriveView.qml
        app/qml/TelltaleBar.qml
        app/qml/TurnIndicator.qml
        app/qml/AlertOverlay.qml
        app/qml/GearIndicator.qml
        app/qml/DegradedModeView.qml
        app/qml/adas/AdasStateZone.qml
        app/qml/adas/TakeoverBanner.qml
        app/qml/adas/PerceptionMiniView.qml
    RESOURCES
        app/resources/resources.qrc
)

target_include_directories(driver_ui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/app/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/safety_core/cpp
)

target_link_libraries(driver_ui PRIVATE
    driver_safety_core
    automotive_platform
    automotive_adas3d
    Qt6::Quick
    Qt6::Quick3D
    Qt6::QuickControls2
)

# Import QML modules
target_compile_definitions(driver_ui PRIVATE
    QML_IMPORT_PATH="${CMAKE_BINARY_DIR}"
)

automotive_safety_settings(driver_ui)

# macOS bundle settings
set_target_properties(driver_ui PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.automotive.driverui"
    MACOSX_BUNDLE_BUNDLE_NAME "Driver UI"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    WIN32_EXECUTABLE TRUE
)

# Installation
install(TARGETS driver_ui
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
