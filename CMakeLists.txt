# CMake build for LinkedListBrowser.
#
# Registration modules (commands, sort algorithms, and structure operations) rely on
# global initializers running at startup. They are compiled into an OBJECT library and
# linked directly into each executable so the linker cannot drop those object files.

cmake_minimum_required(VERSION 3.16)
project(LinkedListBrowser CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /EHsc)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE LLB_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(llb_objects OBJECT ${LLB_SOURCES})

add_executable(LinkedListBrowser
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    $<TARGET_OBJECTS:llb_objects>)

file(GLOB_RECURSE LLB_TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)

add_executable(LinkedListTests
    ${LLB_TEST_SOURCES}
    $<TARGET_OBJECTS:llb_objects>)

enable_testing()
add_test(
    NAME unit_tests
    COMMAND LinkedListTests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
