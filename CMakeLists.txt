cmake_minimum_required(VERSION 3.21)

project(AutomotiveUIProject
    VERSION 1.0.0
    DESCRIPTION "Automotive UI Suite - Driver UI + Infotainment UI"
    LANGUAGES CXX
)

# ============================================================================
# Global Settings
# ============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Build options
option(BUILD_DRIVER_UI "Build Driver UI executable" ON)
option(BUILD_INFOTAINMENT_UI "Build Infotainment UI executable" ON)
option(ENABLE_SIM "Enable simulation/mock adapters" ON)
option(ENABLE_TESTS "Build test suites" ON)
option(ENABLE_STATIC_ANALYSIS "Enable clang-tidy during build" OFF)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# Find Qt6
# ============================================================================

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Network
    Positioning
    Location
)

if(ENABLE_TESTS)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Test QuickTest)
endif()

qt_standard_project_setup()

# ============================================================================
# Compiler Settings
# ============================================================================

# Common warning flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
        -Werror=uninitialized
        -Wno-unused-parameter
    )

    # Safety core: no exceptions (optional, per policy)
    # add_compile_options(-fno-exceptions)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O2)
    endif()
elseif(MSVC)
    add_compile_options(
        /W4
        /WX
        /permissive-
        /Zc:__cplusplus
    )
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Static analysis integration
if(ENABLE_STATIC_ANALYSIS)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY
            ${CLANG_TIDY_EXE}
            --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
        )
    endif()
endif()

# ============================================================================
# Output Directories
# ============================================================================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# Subdirectories
# ============================================================================

# Shared platform libraries
add_subdirectory(shared)

# Driver UI
if(BUILD_DRIVER_UI)
    add_subdirectory(driver_ui)
endif()

# Infotainment UI
if(BUILD_INFOTAINMENT_UI)
    add_subdirectory(infotainment_ui)
endif()

# Tests
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================================
# Installation
# ============================================================================

include(GNUInstallDirs)

install(DIRECTORY shared/qml/
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/automotive-ui/qml
)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "=== Automotive UI Suite Configuration ===")
message(STATUS "Version:              ${PROJECT_VERSION}")
message(STATUS "Build type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:         ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt Version:           ${Qt6_VERSION}")
message(STATUS "")
message(STATUS "Components:")
message(STATUS "  Driver UI:          ${BUILD_DRIVER_UI}")
message(STATUS "  Infotainment UI:    ${BUILD_INFOTAINMENT_UI}")
message(STATUS "  Simulation:         ${ENABLE_SIM}")
message(STATUS "  Tests:              ${ENABLE_TESTS}")
message(STATUS "  Static Analysis:    ${ENABLE_STATIC_ANALYSIS}")
message(STATUS "==========================================")
message(STATUS "")
