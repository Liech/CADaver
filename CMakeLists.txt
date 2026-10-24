cmake_minimum_required(VERSION 3.20...3.30)
project(Solution)

include(FetchContent)

#https://thatonegamedev.com/cpp/cmake/godot-4-gdextension-for-c-using-cmake/
FetchContent_Declare(
        GDExtension
        GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
        GIT_TAG godot-4.5-stable # <- to change the compatible godot version just change this tag
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/out/_deps/gdextension-src")
    set(FETCHCONTENT_SOURCE_DIR_GDEXTENSION "${CMAKE_CURRENT_SOURCE_DIR}/out/_deps/gdextension-src")
endif()

if(NOT DEFINED Python3_EXECUTABLE)
    find_program(Python3_EXECUTABLE NAMES python python3 PATHS
        "$ENV{LOCALAPPDATA}/Programs/Python/Python313"
        "$ENV{LOCALAPPDATA}/Programs/Python/Python312"
        "$ENV{LOCALAPPDATA}/Programs/Python/Python311"
        "C:/msys64/mingw64/bin"
        NO_DEFAULT_PATH
    )
endif()

set(GODOTCPP_USE_STATIC_CPP OFF CACHE BOOL "Use dynamic CRT" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "" FORCE)

FetchContent_MakeAvailable(GDExtension)

if(NOT TARGET CADacomb::CADacomb)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../CADacomb" "${CMAKE_CURRENT_BINARY_DIR}/CADacomb")
endif()

add_subdirectory(Executable)
add_subdirectory(Library)
add_subdirectory(LibraryGD)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT CadaverExe)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT CadaverExe)
