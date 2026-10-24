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
    "Source/Renderer.h"
    "Source/Renderer.cpp"
    "Source/FileMenu.h"
    "Source/FileMenu.cpp"
)
source_group("Source" FILES ${Source_Files})

set(Preferences_Files
    "Source/Preferences/Preferences.h"
    "Source/Preferences/Preferences.cpp"
    "Source/Preferences/UIPreferences.h"
    "Source/Preferences/UIPreferences.cpp"
    "Source/Preferences/CameraPreferences.h"
    "Source/Preferences/CameraPreferences.cpp"
    "Source/Preferences/RenderPreferences.h"
    "Source/Preferences/RenderPreferences.cpp"
)
source_group("Source/Preferences" FILES ${Preferences_Files})

set(UI_Preferences_Files
    "Source/UI/Preferences/PreferencesWindow.h"
    "Source/UI/Preferences/PreferencesWindow.cpp"
    "Source/UI/Preferences/UIPreferencesView.h"
    "Source/UI/Preferences/UIPreferencesView.cpp"
    "Source/UI/Preferences/CameraPreferencesView.h"
    "Source/UI/Preferences/CameraPreferencesView.cpp"
    "Source/UI/Preferences/RenderPreferencesView.h"
    "Source/UI/Preferences/RenderPreferencesView.cpp"
)
source_group("Source/UI/Preferences" FILES ${UI_Preferences_Files})

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

set(Rendering_Files
    "Source/Rendering/AssetRenderer.h"
    "Source/Rendering/AssetRenderer.cpp"
    "Source/Rendering/TriangulationAssetRenderer.h"
    "Source/Rendering/TriangulationAssetRenderer.cpp"
    "Source/Rendering/StepAssetRenderer.h"
    "Source/Rendering/StepAssetRenderer.cpp"
    "Source/Rendering/AssetRenderFactory.h"
    "Source/Rendering/AssetRenderFactory.cpp"
)
source_group("Source/Rendering" FILES ${Rendering_Files})

set(Database_Files
    "Source/Database/Database.h"
    "Source/Database/Database.cpp"
)
source_group("Source/Database" FILES ${Database_Files})

set(ALL_FILES
    ${Source_Files}
    ${Preferences_Files}
    ${UI_Preferences_Files}
    ${Project_Files}
    ${Rendering_Files}
    ${Database_Files}
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