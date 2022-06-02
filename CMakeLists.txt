cmake_minimum_required(VERSION 3.19)
project(Trapdoor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_VERBOSE_MAKEFILE ON)
file(GLOB_RECURSE SRC_FILES_DIR
        ${PROJECT_SOURCE_DIR}/SDK/Header/*.cpp
        ${PROJECT_SOURCE_DIR}/SDK/Header/*.hpp
        ${PROJECT_SOURCE_DIR}/SDK/Header/*.h
        src/commands/TrapdoorCommand.cpp
        src/commands/SpawnCommand.cpp
        src/commands/TestCommand.cpp
        src/commands/TickCommand.cpp
        src/commands/DrawCommand.cpp
        src/commands/CounterCommand.cpp
        src/commands/HsaCommand.cpp
        src/commands/DataCommand.cpp
        src/commands/LogCommand.cpp
        src/commands/ProfCommand.cpp
        src/commands/HudCommand.cpp
        src/commands/VillageCommand.cpp
        src/commands/TrFunctionCommand.cpp
        src/commands/OSCommand.cpp
        src/commands/CommandHelper.cpp
        src/commands/TrapdoorCommand.cpp
        src/data/TVec3.cpp
        src/data/TBlockPos.cpp
        src/data/TAABB.cpp
        src/data/TActor.cpp
        src/data/DataConverter.cpp
        src/dllmain.cpp
        src/base/TrapdoorMod.cpp
        src/base/Config.cpp
        src/base/Msg.cpp
        src/base/Particle.cpp
        src/pch.cpp
        src/Plugin.cpp
        src/functions/SimpleProfiler.cpp
        src/functions/MCTick.cpp
        src/functions/VillageHelper.cpp
        src/functions/SpawnHelper.cpp
        src/functions/InfoDisplayer.cpp 
        src/functions/HsaHelper.cpp 
        src/functions/HopperCounter.cpp 
        src/functions/HUDHelper.cpp 
        src/events/ItemUseOnEvent.cpp
        src/events/PlayerAttackEvent.cpp
        )

include_directories(SDK/Header)
include_directories(SDK/Header/third-party)
include_directories(src/include)
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
        /MANIFESTUAC:NO /OPT:ICF /ERRORREPORT:PROMPT /NOLOGO  /TLBID:1
)

add_library(trapdoor SHARED ${SRC_FILES_DIR})

add_custom_command(TARGET trapdoor PRE_BUILD
        COMMAND cmd /c ${PROJECT_SOURCE_DIR}/prepareLib.cmd ${PROJECT_SOURCE_DIR}
        COMMENT "Preparing Library"
        )