#"C:\Program Files\CMake\bin\cmake.EXE" --no-warn-unused-cli 
#-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE "-DCMAKE_C_COMPILER:FILEPATH=C:\Program Files\LLVM\bin\clang.exe" "-DCMAKE_CXX_COMPILER:FILEPATH=C:\Program Files\LLVM\bin\clang++.exe" -Sc:/Users/xhy/dev/trapdoor-ll -Bc:/Users/xhy/dev/trapdoor-llild -G "Visual Studio 17 2022"
#Version
cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#variables
set(TRAPDOOR_VERSION 0.22.0)
set(GAME_VERSION 1.19.50.02)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
string(TIMESTAMP BUILD_TIME "%Y-%m-%d %H:%M:%S")
set(BUILD_TIME= "${BUILD_TIME}")

#configurations
option(DEV "Build as developer version" ON)


project(Trapdoor VERSION ${TRAPDOOR_VERSION})

#export configure file
configure_file(${PROJECT_SOURCE_DIR}/src/config.h.in ${PROJECT_SOURCE_DIR}/src/include/config.h)


#choose version to build
if (${DEV} STREQUAL "ON")
    add_definitions(-DDEV)
    message("Build a developer version")
    set(TARGET_NAME trapdoor-dev)
else ()
    message("Build a release version")
    set(TARGET_NAME trapdoor-${TRAPDOOR_VERSION}-${GAME_VERSION})
endif ()


file(GLOB_RECURSE SRC_FILES_DIR
        ${PROJECT_SOURCE_DIR}/SDK-cpp/Header/*.cpp
        ${PROJECT_SOURCE_DIR}/SDK-cpp/Header/*.hpp
        ${PROJECT_SOURCE_DIR}/SDK-cpp/Header/*.h

        src/commands/TriggerCommand.cpp
        src/commands/TrapdoorCommand.cpp
        src/commands/DistanceCommand.cpp
        src/commands/CalCommand.cpp
        src/commands/SpawnCommand.cpp
        src/commands/TestCommand.cpp
        src/commands/TickCommand.cpp
        src/commands/CounterCommand.cpp
        src/commands/HsaCommand.cpp
        src/commands/DataCommand.cpp
        src/commands/LogCommand.cpp
        src/commands/ProfCommand.cpp
        src/commands/HudCommand.cpp
        src/commands/PlayerCommand.cpp
        src/commands/VillageCommand.cpp
        src/commands/TrFunctionCommand.cpp
        src/commands/ShortcutCommand.cpp
        src/commands/CommandHelper.cpp
        src/commands/TrapdoorCommand.cpp
        src/commands/TweakCommand.cpp
        src/commands/SlimeChunkCommand.cpp
        src/commands/RandCommand.cpp
        src/data/TVec3.cpp
        src/data/TBlockPos.cpp
        src/data/TAABB.cpp
        src/data/TActor.cpp
        src/data/DataConverter.cpp
        src/base/TrapdoorMod.cpp
        src/base/Config.cpp
        src/base/Msg.cpp
        src/base/Particle.cpp
        src/base/Utils.cpp
        src/base/TrAPI.cpp

        src/functions/RedstoneTool.cpp
        src/functions/EventTrigger.cpp
        src/functions/DistanceHelper.cpp
        src/functions/SysInfoHelper.cpp
        src/functions/SpawnAnalyzer.cpp
        src/functions/SimpleProfiler.cpp
        src/functions/MCTick.cpp
        src/functions/VillageHelper.cpp
        src/functions/SpawnHelper.cpp
        src/functions/InfoDisplay.cpp
        src/functions/Shortcuts.cpp
        src/functions/HsaHelper.cpp
        src/functions/Events.cpp
        src/functions/HopperCounter.cpp
        src/functions/HUDHelper.cpp
        src/functions/SimPlayerManager.cpp
        src/functions/BlockRotateHelper.cpp
        src/functions/Tweakers.cpp
        src/functions/InventoryTool.cpp
        src/functions/SlimeChunkHelper.cpp
        src/functions/RandomTweaker.cpp
        src/pch.cpp
        src/Plugin.cpp
        src/dllmain.cpp
        )

include_directories(SDK-cpp/include)
include_directories(SDK-cpp/include/llapi)
include_directories(${PROJECT_SOURCE_DIR}/src/include)
link_directories(.)

add_definitions(-D"NDEBUG" -D"TEMPLATE_EXPORTS" -D"WIN32_LEAN_AND_MEAN" -D"_CRT_SECURE_NO_WARNINGS" -D"_WINDOWS"
        -D"_USRDLL" -D"_AMD64_" -D"NOMINMAX" -D"_WINDLL" -D"_UNICODE" -D"UNICODE")

add_compile_options(
        /utf-8
        /permissive- /GS /GL /W3 /Gy /Zc:wchar_t /Zi /Gm- /sdl /Zc:inline /fp:precise /errorReport:prompt /WX-
        /Zc:forScope /Gd /Oi /MD /std:c++17 /FC /EHsc /nologo /diagnostics:column
)

add_link_options(
        /DELAYLOAD:bedrock_server.dll
        /MANIFEST /LTCG:incremental /NXCOMPAT /DEBUG /DLL /MACHINE:X64 /OPT:REF /INCREMENTAL:NO /SUBSYSTEM:WINDOWS
        /MANIFESTUAC:NO /OPT:ICF /ERRORREPORT:PROMPT /NOLOGO /TLBID:1
)

add_library(${TARGET_NAME} SHARED ${SRC_FILES_DIR})

target_link_libraries(${TARGET_NAME} Pdh)

add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
        COMMAND cmd /c ${PROJECT_SOURCE_DIR}/prepareLib.cmd ${PROJECT_SOURCE_DIR}
        COMMENT "Preparing Library"
        )
