cmake_minimum_required(VERSION 2.6)

add_definitions(-std=c++17)

set(VERSION 0.1.1)
set(ARCHIVE_NAME "${CMAKE_PROJECT_NAME}-${VERSION}")

project(srt-server)

# Required packages
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

add_executable(srt-server src/server.cc)

add_custom_target(dist
  COMMAND tar -czvf ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.gz
    --exclude='*.tar.gz'
    --exclude='build'
    --exclude='.git'
    -C ${CMAKE_SOURCE_DIR} .)

OPTION(RUN_CPPCHECK "Run cppcheck" OFF)

IF(RUN_CPPCHECK)
    include(CppcheckTargets)
    add_cppcheck(srt-server)
ENDIF(RUN_CPPCHECK)

message(STATUS "------------------------------")
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Change a values with: cmake -D<Variable>=<Value>")
message(STATUS "------------------------------")
message(STATUS)
