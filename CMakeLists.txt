cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)

project(tic28x CXX)

add_library(${PROJECT_NAME} SHARED
        src/architecture.cpp
        src/architecture.h
        src/flags.h
        src/info.cpp
        src/info.h
        src/instructions.cpp
        src/instructions.h
        src/lift.cpp
        src/lift.h
        src/opcodes.h
        src/registers.h
        src/sizes.h
        src/text.cpp
        src/text.h
        src/util.cpp
        src/util.h
)

target_link_libraries(${PROJECT_NAME}
        binaryninjaapi)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)

set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)

bn_install_plugin(${PROJECT_NAME})

# Install Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        DOWNLOAD_EXTRACT_TIMESTAMP true
        URL https://github.com/google/googletest/archive/4902ea2d7c6faed89b6facee00baa34bb108fc0d.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test Architecture
add_executable(tic28x_architecture_test
        src/instructions_test.cpp
        src/text_test.cpp
        src/text_test.h)
target_link_libraries(tic28x_architecture_test GTest::gtest_main ${PROJECT_NAME}) # link to binaryninjaapi

# Discover Tests
include(GoogleTest)
gtest_discover_tests(tic28x_architecture_test)
