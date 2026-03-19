cmake_minimum_required(VERSION 3.25)
project(EPPA VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Conan-generated dependency paths (rtmidi, nlohmann_json)
# Run from your build directory:
#   conan install ../Config --build=missing
#   cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DPTSL_SDK_DIR=/path/to/PTSL_SDK_CPP.2025.10.0.1267955
# ---------------------------------------------------------------------------
find_package(RtMidi REQUIRED)
find_package(nlohmann_json REQUIRED)

# ---------------------------------------------------------------------------
# PTSL SDK — point this at your extracted SDK root.
# Override via: cmake .. -DPTSL_SDK_DIR=/absolute/path/to/sdk
# ---------------------------------------------------------------------------
set(PTSL_SDK_DIR "" CACHE PATH "Path to the PTSL C++ SDK root directory")

if(NOT PTSL_SDK_DIR)
    message(FATAL_ERROR
        "PTSL_SDK_DIR is not set. "
        "Pass it with: cmake .. -DPTSL_SDK_DIR=/path/to/PTSL_SDK_CPP.2025.10.0.1267955")
endif()

# The SDK ships as a pre-built shared library; locate it.
find_library(PTSL_LIB
    NAMES PTSLC_CPP libPTSLC_CPP
    HINTS "${PTSL_SDK_DIR}/lib" "${PTSL_SDK_DIR}/build/lib"
    REQUIRED)

add_library(PTSLC_CPP SHARED IMPORTED)
set_target_properties(PTSLC_CPP PROPERTIES
    IMPORTED_LOCATION "${PTSL_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${PTSL_SDK_DIR}/Source")

# ---------------------------------------------------------------------------
# EPPA executable
# ---------------------------------------------------------------------------
add_executable(eppa
    src/main.cpp
    src/App.cpp
    src/midi/MidiInputHandler.cpp
    src/pedal/PedalInterpreter.cpp
    src/pedal/PedalStateMachine.cpp
    src/controller/ProToolsController.cpp)

target_include_directories(eppa PRIVATE src)

target_link_libraries(eppa PRIVATE
    PTSLC_CPP
    RtMidi::rtmidi
    nlohmann_json::nlohmann_json)
