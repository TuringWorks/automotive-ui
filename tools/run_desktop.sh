#!/bin/bash
# Run desktop builds of the automotive UI suite
# Usage: ./run_desktop.sh [driver|infotainment|both]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}  Automotive UI Suite - Desktop Runner${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
}

print_step() {
    echo -e "${GREEN}[*]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

check_dependencies() {
    print_step "Checking dependencies..."

    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is not installed. Please install CMake 3.21+."
        exit 1
    fi

    # Check for Qt
    if ! command -v qmake &> /dev/null && ! command -v qmake6 &> /dev/null; then
        print_warning "qmake not found in PATH. Make sure Qt 6.5+ is installed."
        print_warning "You may need to set CMAKE_PREFIX_PATH to your Qt installation."
    fi

    # Check for compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        print_error "No C++ compiler found. Please install GCC or Clang."
        exit 1
    fi

    print_step "Dependencies OK"
}

configure_build() {
    print_step "Configuring build..."

    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    cmake "$PROJECT_ROOT" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DBUILD_DRIVER_UI=ON \
        -DBUILD_INFOTAINMENT_UI=ON \
        -DENABLE_SIM=ON \
        -DENABLE_TESTS=ON

    print_step "Configuration complete"
}

build_project() {
    print_step "Building project..."

    cd "$BUILD_DIR"
    cmake --build . --parallel $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

    print_step "Build complete"
}

run_driver_ui() {
    print_step "Starting Driver UI (Cluster)..."

    if [ -f "$BUILD_DIR/driver_ui/driver_ui" ]; then
        "$BUILD_DIR/driver_ui/driver_ui" &
        DRIVER_PID=$!
        echo "Driver UI started (PID: $DRIVER_PID)"
    else
        print_error "Driver UI binary not found. Did the build succeed?"
        return 1
    fi
}

run_infotainment_ui() {
    print_step "Starting Infotainment UI..."

    if [ -f "$BUILD_DIR/infotainment_ui/infotainment_ui" ]; then
        "$BUILD_DIR/infotainment_ui/infotainment_ui" &
        INFOTAINMENT_PID=$!
        echo "Infotainment UI started (PID: $INFOTAINMENT_PID)"
    else
        print_error "Infotainment UI binary not found. Did the build succeed?"
        return 1
    fi
}

run_tests() {
    print_step "Running tests..."

    cd "$BUILD_DIR"
    ctest --output-on-failure

    print_step "Tests complete"
}

show_usage() {
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  driver        Build and run Driver UI only"
    echo "  infotainment  Build and run Infotainment UI only"
    echo "  both          Build and run both UIs (default)"
    echo "  build         Build without running"
    echo "  test          Build and run tests"
    echo "  clean         Clean build directory"
    echo "  help          Show this help message"
    echo ""
    echo "Environment Variables:"
    echo "  CMAKE_PREFIX_PATH   Path to Qt installation"
    echo "  BUILD_TYPE          Build type (Debug/Release, default: Debug)"
    echo ""
}

cleanup() {
    print_step "Cleaning up..."
    if [ -n "$DRIVER_PID" ]; then
        kill $DRIVER_PID 2>/dev/null || true
    fi
    if [ -n "$INFOTAINMENT_PID" ]; then
        kill $INFOTAINMENT_PID 2>/dev/null || true
    fi
}

trap cleanup EXIT

# Main script
print_header

case "${1:-both}" in
    driver)
        check_dependencies
        configure_build
        build_project
        run_driver_ui
        echo ""
        print_step "Press Ctrl+C to stop..."
        wait
        ;;
    infotainment)
        check_dependencies
        configure_build
        build_project
        run_infotainment_ui
        echo ""
        print_step "Press Ctrl+C to stop..."
        wait
        ;;
    both)
        check_dependencies
        configure_build
        build_project
        run_driver_ui
        sleep 1
        run_infotainment_ui
        echo ""
        print_step "Press Ctrl+C to stop both UIs..."
        wait
        ;;
    build)
        check_dependencies
        configure_build
        build_project
        print_step "Build complete. Run './run_desktop.sh' to start UIs."
        ;;
    test)
        check_dependencies
        configure_build
        build_project
        run_tests
        ;;
    clean)
        print_step "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
        print_step "Clean complete"
        ;;
    help|--help|-h)
        show_usage
        ;;
    *)
        print_error "Unknown option: $1"
        show_usage
        exit 1
        ;;
esac
