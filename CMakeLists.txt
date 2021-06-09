cmake_minimum_required(VERSION 2.6)

add_definitions(-std=c++17)

set(VERSION 0.1.1)
set(ARCHIVE_NAME "${CMAKE_PROJECT_NAME}-${VERSION}")

project(srt-server)

# Required packages
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(SRT REQUIRED)
include_directories(${SRT_SERVER_SOURCE_DIR} ${LIBSRT_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})

add_executable(srt-server
    src/srt_server.cc
    src/common.cc
    src/conf.cc
    src/HttpClient.cc
    src/HttpRoleList.cc
    src/SLSArray.cc
    src/SLSEpollThread.cc
    src/SLSGroup.cc
    src/SLSListener.cc
    src/SLSLog.cc
    src/SLSManager.cc
    src/SLSMapData.cc
    src/SLSMapPublisher.cc
    src/SLSMapRelay.cc
    src/SLSPlayer.cc
    src/SLSPublisher.cc
    src/SLSPuller.cc
    src/SLSPullerManager.cc
    src/SLSPusher.cc
    src/SLSPusherManager.cc
    src/SLSRecycleArray.cc
    src/SLSRelay.cc
    src/SLSRelayManager.cc
    src/SLSRole.cc
    src/SLSRoleList.cc
    src/SLSSrt.cc
    src/SLSSyncClock.cc
    src/SLSThread.cc
    src/TCPRole.cc
    src/TSFileTimeReader.cc)
string(STRIP ${LIBSRT_LIBRARIES} LIBSRT_LIBRARIES)
target_link_libraries(srt-server ${CMAKE_THREAD_LIBS_INIT} ${LIBSRT_LIBRARIES} ${ZLIB_LIBRARIES})

add_custom_target(dist
  COMMAND tar -czvf ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.gz
    --exclude='*.tar.gz'
    --exclude='build'
    --exclude='.git'
    -C ${CMAKE_SOURCE_DIR} .)

INSTALL(TARGETS srt-server
    RUNTIME DESTINATION bin
)

OPTION(RUN_CPPCHECK "Run cppcheck" OFF)

IF(RUN_CPPCHECK)
    include(CppcheckTargets)
    add_cppcheck(srt-server)
ENDIF(RUN_CPPCHECK)

message(STATUS "------------------------------")
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "RUN_CPPCHECK = ${RUN_CPPCHECK}")
message(STATUS "Change a values with: cmake -D<Variable>=<Value>")
message(STATUS "------------------------------")
message(STATUS)
