cmake_minimum_required(VERSION 3.15)
project(EPPAClient VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Path to the pre-built PTSLC_CPP framework
set(PTSL_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/PTSL_SDK_CPP.2025.10.0.1267955")
set(PTSL_FRAMEWORK_DIR "${PTSL_SDK_DIR}/install/arm64/Debug/PTSLC_CPP")

set(NLOHMANN_JSON_INCLUDE_DIR
    "${PTSL_SDK_DIR}/Config/.conan/data/nlohmann_json/3.11.3/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include"
)

add_executable(EPPAClient
    main.cpp
    controller/App.cpp
    controller/MidiInputHandler.cpp
    model/PedalInterpreter.cpp
    controller/PlaybackController.cpp
    voiceover/Reader.cpp
    RtMidi.cpp
)

target_compile_definitions(EPPAClient PRIVATE __MACOSX_CORE__)

target_include_directories(EPPAClient PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/model"
    "${CMAKE_CURRENT_SOURCE_DIR}/controller"
    "${CMAKE_CURRENT_SOURCE_DIR}/voiceover"
    "${PTSL_FRAMEWORK_DIR}/PTSLC_CPP.framework/Headers"
    "${NLOHMANN_JSON_INCLUDE_DIR}"
)

target_link_libraries(EPPAClient PRIVATE
    "-F${PTSL_FRAMEWORK_DIR}"
    "-framework PTSLC_CPP"
    "-framework CoreMIDI"
    "-framework CoreAudio"
    "-framework CoreFoundation"
)

# Set rpath so the binary finds the framework at runtime
set_target_properties(EPPAClient PROPERTIES
    BUILD_RPATH "${PTSL_FRAMEWORK_DIR}"
    INSTALL_RPATH "${PTSL_FRAMEWORK_DIR}"
)
