# "C:\Program Files\CMake\bin\cmake.EXE" --no-warn-unused-cli
# -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE "-DCMAKE_C_COMPILER:FILEPATH=C:\Program Files\LLVM\bin\clang.exe" "-DCMAKE_CXX_COMPILER:FILEPATH=C:\Program Files\LLVM\bin\clang++.exe" -Sc:/Users/xhy/dev/trapdoor-ll -Bc:/Users/xhy/dev/trapdoor-llild -G "Visual Studio 17 2022"
# Version
cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# variables
set(TRAPDOOR_VERSION 0.38.0)
set(GAME_VERSION 1.20.0.01)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
string(TIMESTAMP BUILD_TIME "%Y-%m-%d %H:%M:%S")
set(BUILD_TIME= "${BUILD_TIME}")

# configurations
option(DEV "Build as a developer version" ON)
SET(CommitHash "" CACHE STRING "CommitHash")
project(Trapdoor VERSION ${TRAPDOOR_VERSION})

# export configure file
configure_file(${PROJECT_SOURCE_DIR}/src/config.h.in ${PROJECT_SOURCE_DIR}/src/include/config.h)

# choose version to build
if (${DEV} STREQUAL "ON")
    add_definitions(-DDEV)
    message("Build a developer version")
    set(TARGET_NAME trapdoor-dev)
else ()
    message("Build a release version")

    if (${CommitHash} STREQUAL "")
        set(TARGET_NAME trapdoor-${TRAPDOOR_VERSION}-${GAME_VERSION})
    else ()
        set(TARGET_NAME trapdoor-${TRAPDOOR_VERSION}-${GAME_VERSION}-${CommitHash})
    endif ()
endif ()

file(GLOB_RECURSE SRC_FILES_DIR
        src/*/*.cpp
        src/*.cpp
        )

include_directories(SDK-cpp/include)
include_directories(SDK-cpp/include/llapi)
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/src/include/3rd)
include_directories(${PROJECT_SOURCE_DIR}/src/include/3rd/sol)
include_directories(${PROJECT_SOURCE_DIR}/src/include/3rd/lua)

link_directories(.)

add_definitions(-D"NDEBUG" -D"TEMPLATE_EXPORTS" -D"WIN32_LEAN_AND_MEAN" -D"_CRT_SECURE_NO_WARNINGS" -D"_WINDOWS"
        -D"_USRDLL" -D"_AMD64_" -D"NOMINMAX" -D"_WINDLL" -D"_UNICODE" -D"UNICODE")

add_compile_options(
        /utf-8
        /permissive- /GS /GL /W3 /Gy /Zc:wchar_t /Zi /Gm- /sdl /Zc:inline /fp:precise /errorReport:prompt /WX-
        /Zc:forScope /Gd /Oi /MD /std:c++17 /FC /EHsc /nologo /diagnostics:column /MP
)

add_link_options(
        /DELAYLOAD:bedrock_server.dll
        /MANIFEST /LTCG:incremental /NXCOMPAT /DEBUG /DLL /MACHINE:X64 /OPT:REF /INCREMENTAL:NO /SUBSYSTEM:WINDOWS
        /MANIFESTUAC:NO /OPT:ICF /ERRORREPORT:PROMPT /NOLOGO /TLBID:1
)

add_library(${TARGET_NAME} SHARED ${SRC_FILES_DIR})

#后面LUA可以调整成dll
target_link_libraries(${TARGET_NAME} Pdh ${PROJECT_SOURCE_DIR}/src/include/3rd/lua54.lib)

add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
        COMMAND cmd /c ${PROJECT_SOURCE_DIR}/prepareLib.cmd ${PROJECT_SOURCE_DIR}
        COMMENT "Preparing Library"
        )
