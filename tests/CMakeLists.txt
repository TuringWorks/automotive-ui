# Tests CMakeLists.txt

enable_testing()

find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Test helper library
add_library(test_helpers STATIC
    helpers/MockSignalHub.cpp
    helpers/MockTimeSource.cpp
    helpers/TestFixtures.cpp
)

target_include_directories(test_helpers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/helpers
)

target_link_libraries(test_helpers PUBLIC
    GTest::gtest
    GTest::gmock
    automotive_shared
    Qt6::Core
)

# Safety core tests
add_executable(test_safety_core
    safety/test_signal_validator.cpp
    safety/test_cluster_state_model.cpp
    safety/test_alert_manager.cpp
    safety/test_degraded_mode.cpp
    safety/test_fault_injector.cpp
)

target_link_libraries(test_safety_core PRIVATE
    test_helpers
    GTest::gtest_main
    GTest::gmock
    driver_ui_safety_core
)

add_test(NAME SafetyCoreTests COMMAND test_safety_core)

# Security tests
add_executable(test_security
    security/test_permission_manager.cpp
    security/test_pii_redactor.cpp
    security/test_ipc_integrity.cpp
    security/test_secure_settings.cpp
)

target_link_libraries(test_security PRIVATE
    test_helpers
    GTest::gtest_main
    GTest::gmock
    infotainment_ui_core
)

add_test(NAME SecurityTests COMMAND test_security)

# IPC tests
add_executable(test_ipc
    ipc/test_ipc_message.cpp
    ipc/test_ipc_channel.cpp
)

target_link_libraries(test_ipc PRIVATE
    test_helpers
    GTest::gtest_main
    automotive_shared
)

add_test(NAME IPCTests COMMAND test_ipc)

# Integration tests
add_executable(test_integration
    integration/test_signal_flow.cpp
    integration/test_ui_responsiveness.cpp
)

target_link_libraries(test_integration PRIVATE
    test_helpers
    GTest::gtest_main
    GTest::gmock
    driver_ui_safety_core
    infotainment_ui_core
)

add_test(NAME IntegrationTests COMMAND test_integration)

# Coverage target
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    find_program(GCOV gcov)
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)

    if(GCOV AND LCOV AND GENHTML)
        add_custom_target(coverage
            COMMAND ${LCOV} --directory . --zerocounters
            COMMAND ctest --output-on-failure
            COMMAND ${LCOV} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV} --remove coverage.info '/usr/*' '*/tests/*' '*/build/*' --output-file coverage.info
            COMMAND ${GENHTML} coverage.info --output-directory coverage_report
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )
    endif()
endif()
