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
)
source_group("Source" FILES ${Source_Files})

set(Database_Preferences_Files
    "Source/Database/Preferences/Preferences.h"
    "Source/Database/Preferences/Preferences.cpp"
    "Source/Database/Preferences/UIPreferences.h"
    "Source/Database/Preferences/UIPreferences.cpp"
    "Source/Database/Preferences/CameraPreferences.h"
    "Source/Database/Preferences/CameraPreferences.cpp"
    "Source/Database/Preferences/RenderPreferences.h"
    "Source/Database/Preferences/RenderPreferences.cpp"
    "Source/Database/Preferences/SVOPreferences.h"
    "Source/Database/Preferences/SVOPreferences.cpp"
)
source_group("Source/Database/Preferences" FILES ${Database_Preferences_Files})

set(UI_Preferences_Files
    "Source/UI/Preferences/PreferencesWindow.h"
    "Source/UI/Preferences/PreferencesWindow.cpp"
    "Source/UI/Preferences/UIPreferencesView.h"
    "Source/UI/Preferences/UIPreferencesView.cpp"
    "Source/UI/Preferences/CameraPreferencesView.h"
    "Source/UI/Preferences/CameraPreferencesView.cpp"
    "Source/UI/Preferences/RenderPreferencesView.h"
    "Source/UI/Preferences/RenderPreferencesView.cpp"
    "Source/UI/Preferences/SVOPreferencesView.h"
    "Source/UI/Preferences/SVOPreferencesView.cpp"
)
source_group("Source/UI/Preferences" FILES ${UI_Preferences_Files})

set(Database_Files
    "Source/Database/Database.h"
    "Source/Database/Database.cpp"
    "Source/Database/Project.h"
    "Source/Database/Project.cpp"
    "Source/Database/Asset.h"
    "Source/Database/Asset.cpp"
    "Source/Database/AssetType.h"
    "Source/Database/AssetType.cpp"
    "Source/Database/AssetConverter.h"
    "Source/Database/AssetConverter.cpp"
)
source_group("Source/Database" FILES ${Database_Files})

set(Database_Asset_Files
    "Source/Database/Asset/CadAsset.h"
    "Source/Database/Asset/CadAsset.cpp"
    "Source/Database/Asset/TriangulationAsset.h"
    "Source/Database/Asset/TriangulationAsset.cpp"
    "Source/Database/Asset/VoxelAsset.h"
    "Source/Database/Asset/VoxelAsset.cpp"
)
source_group("Source/Database/Asset" FILES ${Database_Asset_Files})

set(Rendering_Files
    "Source/Rendering/Renderer.h"
    "Source/Rendering/Renderer.cpp"
    "Source/Rendering/AssetRenderer.h"
    "Source/Rendering/AssetRenderer.cpp"
    "Source/Rendering/TriangulationAssetRenderer.h"
    "Source/Rendering/TriangulationAssetRenderer.cpp"
    "Source/Rendering/StepAssetRenderer.h"
    "Source/Rendering/StepAssetRenderer.cpp"
    "Source/Rendering/AssetRenderFactory.h"
    "Source/Rendering/AssetRenderFactory.cpp"
    "Source/Rendering/VoxelAssetRenderer.h"
    "Source/Rendering/VoxelAssetRenderer.cpp"
)
source_group("Source/Rendering" FILES ${Rendering_Files})

set(Command_Files
    "Source/Commands/RULES.md"
    "Source/Commands/Command.h"
    "Source/Commands/Command.cpp"
    "Source/Commands/CommandFactory.h"
    "Source/Commands/CommandFactory.cpp"
    "Source/Commands/GatedDatabase.h"
    "Source/Commands/GatedDatabase.cpp"
    "Source/Commands/RemoveAssetCommand.h"
    "Source/Commands/RemoveAssetCommand.cpp"
    "Source/Commands/ClearProjectCommand.h"
    "Source/Commands/ClearProjectCommand.cpp"
    "Source/Commands/ConvertCadToStlCommand.h"
    "Source/Commands/ConvertCadToStlCommand.cpp"
    "Source/Commands/ConvertCadToSvoCommand.h"
    "Source/Commands/ConvertCadToSvoCommand.cpp"
    "Source/Commands/ConvertStlToSvoCommand.h"
    "Source/Commands/ConvertStlToSvoCommand.cpp"
    "Source/Commands/ConvertSvoToStlCommand.h"
    "Source/Commands/ConvertSvoToStlCommand.cpp"
    "Source/Commands/LoadStlAssetCommand.h"
    "Source/Commands/LoadStlAssetCommand.cpp"
    "Source/Commands/SaveStlAssetCommand.h"
    "Source/Commands/SaveStlAssetCommand.cpp"
    "Source/Commands/LoadStepAssetCommand.h"
    "Source/Commands/LoadStepAssetCommand.cpp"
    "Source/Commands/SaveStepAssetCommand.h"
    "Source/Commands/SaveStepAssetCommand.cpp"
    "Source/Commands/LoadSvoAssetCommand.h"
    "Source/Commands/LoadSvoAssetCommand.cpp"
    "Source/Commands/SaveVoxelAssetCommand.h"
    "Source/Commands/SaveVoxelAssetCommand.cpp"
    "Source/Commands/UpdateCameraPreferencesCommand.h"
    "Source/Commands/UpdateCameraPreferencesCommand.cpp"
    "Source/Commands/UpdateUIPreferencesCommand.h"
    "Source/Commands/UpdateUIPreferencesCommand.cpp"
    "Source/Commands/UpdateRenderPreferencesCommand.h"
    "Source/Commands/UpdateRenderPreferencesCommand.cpp"
    "Source/Commands/UpdateSVOPreferencesCommand.h"
    "Source/Commands/UpdateSVOPreferencesCommand.cpp"
)
source_group("Source/Commands" FILES ${Command_Files})

set(UI_About_Files
    "Source/UI/About/AboutWindow.h"
    "Source/UI/About/AboutWindow.cpp"
)
source_group("Source/UI/About" FILES ${UI_About_Files})

set(UI_Commands_Files
    "Source/UI/Commands/CommandsWindow.h"
    "Source/UI/Commands/CommandsWindow.cpp"
)
source_group("Source/UI/Commands" FILES ${UI_Commands_Files})

set(UI_ContextMenu_Files
    "Source/UI/ContextMenu/AssetContextMenu.h"
    "Source/UI/ContextMenu/AssetContextMenu.cpp"
    "Source/UI/ContextMenu/TriangulationAssetContextMenu.h"
    "Source/UI/ContextMenu/TriangulationAssetContextMenu.cpp"
    "Source/UI/ContextMenu/CadAssetContextMenu.h"
    "Source/UI/ContextMenu/CadAssetContextMenu.cpp"
    "Source/UI/ContextMenu/VoxelAssetContextMenu.h"
    "Source/UI/ContextMenu/VoxelAssetContextMenu.cpp"
    "Source/UI/ContextMenu/AssetContextMenuFactory.h"
    "Source/UI/ContextMenu/AssetContextMenuFactory.cpp"
)
source_group("Source/UI/ContextMenu" FILES ${UI_ContextMenu_Files})

set(UI_Project_Files
    "Source/UI/Project/ProjectWindow.h"
    "Source/UI/Project/ProjectWindow.cpp"
)
source_group("Source/UI/Project" FILES ${UI_Project_Files})

set(UI_Menu_Files
    "Source/UI/Menu/FileMenu.h"
    "Source/UI/Menu/FileMenu.cpp"
)
source_group("Source/UI/Menu" FILES ${UI_Menu_Files})

set(ALL_FILES
    ${Source_Files}
    ${Database_Files}
    ${Database_Asset_Files}
    ${Database_Preferences_Files}
    ${Rendering_Files}
    ${UI_Menu_Files}
    ${UI_Preferences_Files}
    ${UI_About_Files}
    ${UI_Commands_Files}
    ${UI_ContextMenu_Files}
    ${UI_Project_Files}
    ${Command_Files}
)

add_executable(CadaverExe ${ALL_FILES})
set_property(TARGET CadaverExe PROPERTY CXX_STANDARD 20)

target_link_libraries(CadaverExe
    glm::glm
    nlohmann_json::nlohmann_json
    CADacombRender
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
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/../about.txt"
        "$<TARGET_FILE_DIR:CadaverExe>/about.txt"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/../Licenses"
        "$<TARGET_FILE_DIR:CadaverExe>/Licenses"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/../about.txt"
        "${CMAKE_CURRENT_BINARY_DIR}/about.txt"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/../Licenses"
        "${CMAKE_CURRENT_BINARY_DIR}/Licenses"
)

set_property(TARGET CadaverExe PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:CadaverExe>")
set_property(TARGET CadaverExe PROPERTY VS_DEBUGGER_ENVIRONMENT 
    "PATH=$<IF:$<CONFIG:Debug>,${CMAKE_CURRENT_SOURCE_DIR}/../out/vcpkg_installed/x64-windows/debug/bin,${CMAKE_CURRENT_SOURCE_DIR}/../out/vcpkg_installed/x64-windows/bin>;$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug/bin,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin>;%PATH%"
)
