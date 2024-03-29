﻿cmake_minimum_required(VERSION 3.5)

project(SchedulerLib LANGUAGES CXX)
set(CMAKE_DEBUG_POSTFIX d)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#
add_library(${PROJECT_NAME} SHARED
    scheduler_global.h
    configcenter.h
    configcenter.cpp
    core.h
    core.cpp
    module.h
    module.cpp
    ddsnetwork.h
    ddsnetwork.cpp
    layeredlayout.h
    layeredlayout.cpp
    packetutility.h
    packetutility.cpp
    packetutilityv2.h
    packetutilityv2.cpp
    packetutilityv3.h
    packetutilityv3.cpp
    udppacketutility.h
    udppacketutility.cpp
    )
add_library(Lib::Scheduler ALIAS ${PROJECT_NAME})

#
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network Positioning)
set(DDS_ROOT ${PROJECT_SOURCE_DIR}/3rdParty/CloudBus)

#
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${DDS_ROOT}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Positioning)
target_link_libraries(${PROJECT_NAME} PUBLIC debug ${DDS_ROOT}/x64/VS2013/CloudBusd.lib optimized ${DDS_ROOT}/x64/VS2013/CloudBus.lib)
target_compile_definitions(${PROJECT_NAME} PRIVATE SCHEDULER_LIBRARY)

#
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION install)
install(FILES ${DDS_ROOT}/x64/VS2013/CloudBusd.dll CONFIGURATIONS Debug DESTINATION install)
install(FILES ${DDS_ROOT}/x64/VS2013/CloudBus.dll CONFIGURATIONS Release DESTINATION install)
