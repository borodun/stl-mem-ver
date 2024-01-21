cmake_minimum_required(VERSION 3.12)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Catch2 as a submodule
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# Library settings
set(LIBNAME memver)
set(LIB_DIR lib)
set(LIBFILE ${LIB_DIR}/lib${LIBNAME}.so)
file(GLOB LIB_SOURCES ${LIB_DIR}/*.cpp)
add_library(${LIBNAME} SHARED ${LIB_SOURCES})
target_include_directories(${LIBNAME} PRIVATE ${LIB_DIR}/include)

# Executable settings
set(PROGNAME tests)
set(PROG_DIR test)
file(GLOB PROG_SOURCES ${PROG_DIR}/catch2.cpp)
add_executable(${PROGNAME} ${PROG_SOURCES})
target_include_directories(${PROGNAME} PRIVATE ${PROG_DIR}/include ${LIB_DIR}/include)
target_link_libraries(${PROGNAME} PRIVATE ${LIBNAME} Catch2::Catch2WithMain)

# Compiler flags
target_compile_options(${LIBNAME} PRIVATE
    -Wall -Wpointer-arith -Werror=vla -Wendif-labels -Wmissing-format-attribute
    -Wimplicit-fallthrough=3 -Wcast-function-type -Wshadow=compatible-local
    -Wformat-security -fno-strict-aliasing -fwrapv -Wno-format-truncation
    -Wno-stringop-truncation -g -Og -ggdb3 -fno-omit-frame-pointer -fPIC -std=c++20
    -fconcepts-diagnostics-depth=2
    # -fsanitize=address
)

# Testing
include(CTest)
include(Catch)
catch_discover_tests(${PROGNAME})

# Custom target for running the tests
add_custom_target(run-tests
    COMMAND ${PROGNAME} -s
    DEPENDS ${PROGNAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Custom target for documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.conf
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif()

# Clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/clean-all.cmake
)
