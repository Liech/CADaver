cmake_minimum_required(VERSION 2.8...3.20)
project(CadaverExe)

find_package(nlohmann_json REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(nfd CONFIG REQUIRED)
if(NOT TARGET Vulcant::Vulcant)
    find_package(Vulcant CONFIG REQUIRED)
endif()

set(Source_Files
    "Source/main.cpp"
    "Source/Cadaver.h"
    "Source/Cadaver.cpp"
)
source_group("Source" FILES ${Source_Files})

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
    "Source/UI/Preferences/UndoPreferencesView.h"
    "Source/UI/Preferences/UndoPreferencesView.cpp"
)
source_group("Source/UI/Preferences" FILES ${UI_Preferences_Files})

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

set(UI_Component_Files
    "Source/UI/Component/VoxelResolutionComboBox.h"
    "Source/UI/Component/VoxelResolutionComboBox.cpp"
    "Source/UI/Component/ViewCubeWidget.h"
    "Source/UI/Component/ViewCubeWidget.cpp"
    "Source/UI/Component/ModalProgressBar.h"
    "Source/UI/Component/ModalProgressBar.cpp"
)
source_group("Source/UI/Component" FILES ${UI_Component_Files})

set(UI_Generator_Files
    "Source/UI/Generator/LandscapeGeneratorWindow.h"
    "Source/UI/Generator/LandscapeGeneratorWindow.cpp"
)
source_group("Source/UI/Generator" FILES ${UI_Generator_Files})

set(UI_Converter_Files
    "Source/UI/Converter/CadToStlConversionModal.h"
    "Source/UI/Converter/CadToStlConversionModal.cpp"
    "Source/UI/Converter/CadToSvoConversionModal.h"
    "Source/UI/Converter/CadToSvoConversionModal.cpp"
    "Source/UI/Converter/StlToSvoConversionModal.h"
    "Source/UI/Converter/StlToSvoConversionModal.cpp"
    "Source/UI/Converter/SvoToStlConversionModal.h"
    "Source/UI/Converter/SvoToStlConversionModal.cpp"
)
source_group("Source/UI/Converter" FILES ${UI_Converter_Files})

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
    "Source/UI/Menu/TopMenu.h"
    "Source/UI/Menu/TopMenu.cpp"
    "Source/UI/Menu/FileMenu.h"
    "Source/UI/Menu/FileMenu.cpp"
    "Source/UI/Menu/EditMenu.h"
    "Source/UI/Menu/EditMenu.cpp"
    "Source/UI/Menu/CreateMenu.h"
    "Source/UI/Menu/CreateMenu.cpp"
)
source_group("Source/UI/Menu" FILES ${UI_Menu_Files})

set(Camera_Files
    "Source/Camera/CameraController.h"
    "Source/Camera/CameraController.cpp"
    "Source/Camera/ViewController.h"
    "Source/Camera/ViewController.cpp"
)
source_group("Source/Camera" FILES ${Camera_Files})

set(UI_Dialog_Files
    "Source/UI/Dialog/LoadFileDialog.cpp"
    "Source/UI/Dialog/LoadFileDialog.h"
    "Source/UI/Dialog/SaveFileDialog.cpp"
    "Source/UI/Dialog/SaveFileDialog.h"
)
source_group("Source/UI/Dialog" FILES ${UI_Dialog_Files})

set(ALL_FILES
    ${UI_Dialog_Files}
    ${Source_Files}
    ${Camera_Files}
    ${Rendering_Files}
    ${UI_Menu_Files}
    ${UI_Preferences_Files}
    ${UI_About_Files}
    ${UI_Commands_Files}
    ${UI_Component_Files}
    ${UI_Generator_Files}
    ${UI_Converter_Files}
    ${UI_ContextMenu_Files}
    ${UI_Project_Files}
)

add_executable(CadaverExe ${ALL_FILES})
set_property(TARGET CadaverExe PROPERTY CXX_STANDARD 20)

target_link_libraries(CadaverExe
    glm::glm
    nlohmann_json::nlohmann_json
    CADacombRender
    CadaverLibrary
    CadaverUI
    nfd::nfd
    Vulcant::Vulcant
    VulcantToolkit
    imgui::imgui
    imguizmo::imguizmo
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
