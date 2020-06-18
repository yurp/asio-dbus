# Copyright (c) Benjamin Kietzman (github.com/bkietz)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

###############
# CMake options
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(HUNTER_ENABLED "Enable hunter package pulling" OFF)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.18.64.tar.gz"
    SHA1 "baf9c8cc4f65306f0e442b5419967b4c4c04589a"
)

project(asio-dbus CXX)

###############
# C++ options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)

###############
# import D-Bus
hunter_add_package(dbus)
find_package(dbus REQUIRED) # Include functions provided by PkgConfig module.
include_directories(${DBUS_INCLUDE_DIRS})

hunter_add_package(pthread-stubs)
find_package (Threads)

##############
# import Asio
add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_HAS_STD_SYSTEM_ERROR)
add_definitions(-DASIO_HEADER_ONLY)

##############
# Install
add_library(asio-dbus INTERFACE)
target_include_directories(asio-dbus INTERFACE
                           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
			   ${DBUS_INCLUDE_DIRS})

target_link_libraries(asio-dbus INTERFACE ${DBUS_LIBRARIES})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

##############
# Tests
enable_testing()

add_executable(dbustests "test/avahi.cpp" "test/message.cpp" "test/error.cpp" "test/dbusPropertiesServer.cpp" "test/connection.cpp")

##############
# import GTest
hunter_add_package(GTest)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

if (${HUNTER_ENABLED})
    find_package(GMock CONFIG REQUIRED)
    target_link_libraries(dbustests GMock::main)
else()
    find_package(GMock REQUIRED)
    target_link_libraries(dbustests ${GTEST_BOTH_LIBRARIES} gmock)
endif()
target_link_libraries(dbustests ${CMAKE_THREAD_LIBS_INIT})
add_test(dbustests dbustests "--gtest_output=xml:${test_name}.xml")

target_link_libraries(dbustests asio-dbus)


# export targets for find_package config mode
export(TARGETS asio-dbus
       FILE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake)

file(WRITE
    ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
    "include(\${CMAKE_CURRENT_LIST_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake)\n")

install(FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        DESTINATION lib/cmake/${CMAKE_PROJECT_NAME})

install(TARGETS asio-dbus
        EXPORT ${CMAKE_PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
	DESTINATION lib/cmake/${CMAKE_PROJECT_NAME})
