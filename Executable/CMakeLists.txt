cmake_minimum_required(VERSION 2.8...3.20)
project(CadaverExe)

find_package(nlohmann_json REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui REQUIRED)
find_package(Vulcant CONFIG REQUIRED)

set(Source_Files
    "Source/main.cpp"
    "Source/Cadaver.h"
    "Source/Cadaver.cpp"
    "Source/PreferencesData.h"
    "Source/PreferencesData.cpp"
    "Source/UIPreferences.h"
    "Source/UIPreferences.cpp"
    "Source/PreferencesWindow.h"
    "Source/PreferencesWindow.cpp"
    "Source/FileMenu.h"
    "Source/FileMenu.cpp"
)
source_group("Source" FILES ${Source_Files})

set(Project_Files
    "Source/Project/AssetType.h"
    "Source/Project/AssetType.cpp"
    "Source/Project/Asset.h"
    "Source/Project/Asset.cpp"
    "Source/Project/TriangulationAsset.h"
    "Source/Project/TriangulationAsset.cpp"
    "Source/Project/VoxelAsset.h"
    "Source/Project/VoxelAsset.cpp"
    "Source/Project/CadAsset.h"
    "Source/Project/CadAsset.cpp"
    "Source/Project/Project.h"
    "Source/Project/Project.cpp"
    "Source/Project/ProjectWindow.h"
    "Source/Project/ProjectWindow.cpp"
)
source_group("Source/Project" FILES ${Project_Files})

set(ALL_FILES
    ${Source_Files}
    ${Project_Files}
)

add_executable(CadaverExe ${ALL_FILES})
set_property(TARGET CadaverExe PROPERTY CXX_STANDARD 20)

target_link_libraries(CadaverExe
    glm::glm
    nlohmann_json::nlohmann_json
    CadaverLibrary
    Vulcant::Vulcant
    VulcantToolkit
    imgui::imgui
    glfw
)

target_include_directories(CadaverExe PUBLIC 
    ./Source/
)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT CadaverExe)

add_custom_command(TARGET CadaverExe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug/bin,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin>"
        $<TARGET_FILE_DIR:CadaverExe>
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<IF:$<CONFIG:Debug>,${CMAKE_CURRENT_SOURCE_DIR}/../out/vcpkg_installed/x64-windows/debug/bin,${CMAKE_CURRENT_SOURCE_DIR}/../out/vcpkg_installed/x64-windows/bin>"
        $<TARGET_FILE_DIR:CadaverExe>
)

set_property(TARGET CadaverExe PROPERTY VS_DEBUGGER_ENVIRONMENT 
    "PATH=$<IF:$<CONFIG:Debug>,${CMAKE_CURRENT_SOURCE_DIR}/../out/vcpkg_installed/x64-windows/debug/bin,${CMAKE_CURRENT_SOURCE_DIR}/../out/vcpkg_installed/x64-windows/bin>;$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug/bin,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin>;%PATH%"
)