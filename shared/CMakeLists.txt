# shared/CMakeLists.txt
# Shared platform libraries

include(${CMAKE_SOURCE_DIR}/cmake/AutomotiveCompilerSettings.cmake)

# ============================================================================
# Signal Library
# ============================================================================

add_library(automotive_signal STATIC
    cpp/signal/SignalHub.cpp
    cpp/signal/SignalValidator.cpp
    cpp/signal/VehicleSignals.cpp
)

target_include_directories(automotive_signal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
)

target_link_libraries(automotive_signal PUBLIC
    Qt6::Core
)

automotive_safety_settings(automotive_signal)

# ============================================================================
# IPC Library
# ============================================================================

add_library(automotive_ipc STATIC
    cpp/ipc/IpcMessage.cpp
    cpp/ipc/IpcChannel.cpp
    cpp/ipc/IpcServer.cpp
    cpp/ipc/IpcClient.cpp
)

target_include_directories(automotive_ipc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
)

target_link_libraries(automotive_ipc PUBLIC
    Qt6::Core
    Qt6::Network
)

automotive_safety_settings(automotive_ipc)

# ============================================================================
# Scheduler Library
# ============================================================================

add_library(automotive_scheduler STATIC
    cpp/sched/DeterministicScheduler.cpp
    cpp/sched/TimeSource.cpp
)

target_include_directories(automotive_scheduler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
)

target_link_libraries(automotive_scheduler PUBLIC
    Qt6::Core
)

automotive_safety_settings(automotive_scheduler)

# ============================================================================
# Logging Library
# ============================================================================

add_library(automotive_logging STATIC
    cpp/logging/Logger.cpp
    cpp/logging/LogSink.cpp
    cpp/logging/PiiRedactor.cpp
)

target_include_directories(automotive_logging PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
)

target_link_libraries(automotive_logging PUBLIC
    Qt6::Core
)

automotive_standard_settings(automotive_logging)

# ============================================================================
# ADAS Library
# ============================================================================

add_library(automotive_adas STATIC
    cpp/adas/AdasStateService.cpp
    cpp/adas/PerceptionModel.cpp
    cpp/adas/TakeoverManager.cpp
    cpp/adas/HmiEventLog.cpp
    cpp/adas/AdasVisualQualityManager.cpp
)

target_include_directories(automotive_adas PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
)

target_link_libraries(automotive_adas PUBLIC
    Qt6::Core
)

automotive_safety_settings(automotive_adas)

# ============================================================================
# Combined Platform Library
# ============================================================================

add_library(automotive_platform INTERFACE)

target_link_libraries(automotive_platform INTERFACE
    automotive_signal
    automotive_ipc
    automotive_scheduler
    automotive_logging
    automotive_adas
)

# ============================================================================
# QML Module
# ============================================================================

qt_add_qml_module(automotive_designsystem
    URI DesignSystem
    VERSION 1.0
    QML_FILES
        qml/DesignSystem/Tokens.qml
        qml/DesignSystem/Typography.qml
        qml/DesignSystem/Colors.qml
        qml/DesignSystem/Spacing.qml
        qml/DesignSystem/Animations.qml
    QML_FILES
        qml/Components/GaugeBase.qml
        qml/Components/SpeedGauge.qml
        qml/Components/ArcGauge.qml
        qml/Components/Telltale.qml
        qml/Components/AlertBanner.qml
        qml/Components/IconButton.qml
        qml/Components/NavigationBar.qml
        qml/Components/Card.qml
        qml/Components/ListItem.qml
        qml/Components/ProgressBar.qml
        qml/Components/Toggle.qml
    RESOURCES
        qml/assets/icons/telltales.svg
        qml/assets/fonts/fonts.qrc
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/DesignSystem
)

target_link_libraries(automotive_designsystem PUBLIC
    Qt6::Quick
    Qt6::QuickControls2
)

# ============================================================================
# ADAS 3D QML Module
# ============================================================================

qt_add_qml_module(automotive_adas3d
    URI Adas3D
    VERSION 1.0
    QML_FILES
        qml/Adas3D/AdasScene3D.qml
        qml/Adas3D/RoadVisualization.qml
        qml/Adas3D/LaneBoundaryLine.qml
        qml/Adas3D/WeatherParticles.qml
        qml/Adas3D/LeadVehicleMarker.qml
        qml/Adas3D/ObjectInstances.qml
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Adas3D
)

target_link_libraries(automotive_adas3d PUBLIC
    Qt6::Quick
    Qt6::Quick3D
    Qt6::Quick3DParticles
)
