cmake_minimum_required(VERSION 3.15)
project(EPPAClient VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Path to the pre-built PTSLC_CPP framework
set(PTSL_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/PTSL_SDK_CPP.2025.10.0.1267955")
set(PTSL_FRAMEWORK_DIR "${PTSL_SDK_DIR}/install/arm64/Debug/PTSLC_CPP")

add_executable(EPPAClient main.cpp)

target_include_directories(EPPAClient PRIVATE
    "${PTSL_FRAMEWORK_DIR}/PTSLC_CPP.framework/Headers"
)

target_link_libraries(EPPAClient PRIVATE
    "-F${PTSL_FRAMEWORK_DIR}"
    "-framework PTSLC_CPP"
)

# Set rpath so the binary finds the framework at runtime
set_target_properties(EPPAClient PROPERTIES
    BUILD_RPATH "${PTSL_FRAMEWORK_DIR}"
    INSTALL_RPATH "${PTSL_FRAMEWORK_DIR}"
)
