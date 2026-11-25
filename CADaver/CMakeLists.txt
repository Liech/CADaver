cmake_minimum_required(VERSION 2.8...3.20)
project(CadaverExe)

find_package(nlohmann_json REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
if(NOT TARGET Vulcant::Vulcant)
    find_package(Vulcant CONFIG REQUIRED)
endif()

set(Source_Files
    "Source/main.cpp"
    "Source/Cadaver.h"
    "Source/Cadaver.cpp"
    "Source/CommandLineArgs.h"
    "Source/CommandLineArgs.cpp"
    "Source/HeadlessScriptRunner.h"
    "Source/HeadlessScriptRunner.cpp"
)
source_group("Source" FILES ${Source_Files})

set(MCP_Files
    "Source/MCP/HeadlessMcpRunner.h"
    "Source/MCP/HeadlessMcpRunner.cpp"
    "Source/MCP/InteractiveMcpRunner.h"
    "Source/MCP/InteractiveMcpRunner.cpp"
)
source_group("Source/MCP" FILES ${MCP_Files})

find_program(SLANG_COMPILER NAMES slangc slangc.exe PATH_SUFFIXES tools/shader-slang REQUIRED)

set(SHADER_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_shaders")
file(MAKE_DIRECTORY ${SHADER_GEN_DIR})
set(GENERATED_SHADER_HEADERS "")

set(Shader_Files
    "Shaders/Triangulation/SurfaceModifier.slang"
    "Shaders/Triangulation/DefaultSurfaceModifier.slang"
    "Shaders/Triangulation/IndexedFaceColorModifier.slang"
    "Shaders/Triangulation/TexturedSurfaceModifier.slang"
    "Shaders/Triangulation/TriangulationCore.slang"
    "Shaders/Triangulation/Triangulation_vert.exe.slang"
    "Shaders/Triangulation/Triangulation_Default_frag.exe.slang"
    "Shaders/Triangulation/Triangulation_IndexedFaceColor_frag.exe.slang"
    "Shaders/Triangulation/Triangulation_Textured_frag.exe.slang"
    "Shaders/Cad/CadOutline_vert.exe.slang"
    "Shaders/Cad/CadOutline_frag.exe.slang"
    "Shaders/Texture/TextureQuad_vert.exe.slang"
    "Shaders/Texture/TextureQuad_frag.exe.slang"
)
source_group("Shaders" FILES ${Shader_Files})

set(Shader_Dependencies
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Triangulation/SurfaceModifier.slang"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Triangulation/DefaultSurfaceModifier.slang"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Triangulation/IndexedFaceColorModifier.slang"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Triangulation/TexturedSurfaceModifier.slang"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Triangulation/TriangulationCore.slang"
)

foreach(SHADER_FILE ${Shader_Files})
    if(SHADER_FILE MATCHES "\\.exe\\.slang$")
        set(ABS_SHADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}")
        file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Shaders" "${ABS_SHADER_PATH}")

        get_filename_component(REL_DIR "${REL_PATH}" DIRECTORY)
        string(REPLACE ".exe.slang" "" BASE_NAME "${REL_PATH}")
        get_filename_component(FILE_NAME_WE "${BASE_NAME}" NAME)

        set(SUB_DIR "${SHADER_GEN_DIR}/${REL_DIR}")
        set(OUTPUT_HEADER "${SUB_DIR}/${FILE_NAME_WE}.h")

        file(MAKE_DIRECTORY "${SUB_DIR}")

        set(STAGE_FLAG "")
        if(SHADER_FILE MATCHES "_vert\\.exe\\.slang$")
            set(STAGE_FLAG "-stage" "vertex")
        elseif(SHADER_FILE MATCHES "_frag\\.exe\\.slang$")
            set(STAGE_FLAG "-stage" "fragment")
        elseif(SHADER_FILE MATCHES "_comp\\.exe\\.slang$")
            set(STAGE_FLAG "-stage" "compute")
        endif()

        add_custom_command(
            OUTPUT "${OUTPUT_HEADER}"
            COMMAND "${SLANG_COMPILER}" "${ABS_SHADER_PATH}"
                    -target spirv
                    -entry main
                    ${STAGE_FLAG}
                    -profile spirv_1_3
                    -source-embed-style u32
                    -source-embed-language cpp
                    -source-embed-name "${FILE_NAME_WE}_spirv"
                    -o "${OUTPUT_HEADER}"
                    -I "${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
                    -I "${CMAKE_CURRENT_SOURCE_DIR}/../../Vulcant/VulcantToolkit/Shaders/ShaderLibrary"
            DEPENDS "${ABS_SHADER_PATH}" ${Shader_Dependencies}
            COMMENT "Compiling Slang Entry Point: ${REL_PATH}"
            VERBATIM
        )

        list(APPEND GENERATED_SHADER_HEADERS "${OUTPUT_HEADER}")
    endif()
endforeach()

set(Rendering_Files
    "Source/Rendering/Renderer.h"
    "Source/Rendering/Renderer.cpp"
    "Source/Rendering/AssetRenderer.h"
    "Source/Rendering/AssetRenderer.cpp"
    "Source/Rendering/TriangulationAssetRenderer.h"
    "Source/Rendering/TriangulationAssetRenderer.cpp"
    "Source/Rendering/Triangulation/TriangulationSurfaceModule.h"
    "Source/Rendering/Triangulation/TriangulationSurfaceModule.cpp"
    "Source/Rendering/Triangulation/DefaultSurfaceModule.h"
    "Source/Rendering/Triangulation/DefaultSurfaceModule.cpp"
    "Source/Rendering/Triangulation/IndexedFaceColorModule.h"
    "Source/Rendering/Triangulation/IndexedFaceColorModule.cpp"
    "Source/Rendering/Triangulation/TexturedSurfaceModule.h"
    "Source/Rendering/Triangulation/TexturedSurfaceModule.cpp"
    "Source/Rendering/AssetMeshParams.h"
    "Source/Rendering/AssetMeshParams.cpp"
    "Source/Rendering/AssetOutlineParams.h"
    "Source/Rendering/AssetOutlineParams.cpp"
    "Source/Rendering/StepAssetRenderer.h"
    "Source/Rendering/StepAssetRenderer.cpp"
    "Source/Rendering/TextureAssetRenderer.h"
    "Source/Rendering/TextureAssetRenderer.cpp"
    "Source/Rendering/VectorGraphicsAssetRenderer.h"
    "Source/Rendering/VectorGraphicsAssetRenderer.cpp"
    "Source/Rendering/AssetRenderFactory.h"
    "Source/Rendering/AssetRenderFactory.cpp"
    "Source/Rendering/VoxelAssetRenderer.h"
    "Source/Rendering/VoxelAssetRenderer.cpp"
)
source_group("Source/Rendering" FILES ${Rendering_Files})

set(UI_Component_Files
    "Source/UI/Component/ViewCubeWidget.h"
    "Source/UI/Component/ViewCubeWidget.cpp"
)
source_group("Source/UI/Component" FILES ${UI_Component_Files})

set(UI_ViewCube_Files
    "Source/UI/ViewCube/ViewCubeHomeMenuEntry.h"
    "Source/UI/ViewCube/ViewCubeHomeMenuEntry.cpp"
    "Source/UI/ViewCube/ViewCubeRollCounterClockwiseMenuEntry.h"
    "Source/UI/ViewCube/ViewCubeRollCounterClockwiseMenuEntry.cpp"
    "Source/UI/ViewCube/ViewCubeRollClockwiseMenuEntry.h"
    "Source/UI/ViewCube/ViewCubeRollClockwiseMenuEntry.cpp"
    "Source/UI/ViewCube/ViewCubeToggleOrthographicMenuEntry.h"
    "Source/UI/ViewCube/ViewCubeToggleOrthographicMenuEntry.cpp"
    "Source/UI/ViewCube/ViewCubeRotateUpMenuEntry.h"
    "Source/UI/ViewCube/ViewCubeRotateUpMenuEntry.cpp"
    "Source/UI/ViewCube/ViewCubeRotateDownMenuEntry.h"
    "Source/UI/ViewCube/ViewCubeRotateDownMenuEntry.cpp"
    "Source/UI/ViewCube/ViewCubeRotateLeftMenuEntry.h"
    "Source/UI/ViewCube/ViewCubeRotateLeftMenuEntry.cpp"
    "Source/UI/ViewCube/ViewCubeRotateRightMenuEntry.h"
    "Source/UI/ViewCube/ViewCubeRotateRightMenuEntry.cpp"
)
source_group("Source/UI/ViewCube" FILES ${UI_ViewCube_Files})

set(UI_ContextMenu_Files
    "Source/UI/ContextMenu/AssetContextMenu.h"
    "Source/UI/ContextMenu/AssetContextMenu.cpp"
    "Source/UI/ContextMenu/TriangulationAssetContextMenu.h"
    "Source/UI/ContextMenu/TriangulationAssetContextMenu.cpp"
    "Source/UI/ContextMenu/CadAssetContextMenu.h"
    "Source/UI/ContextMenu/CadAssetContextMenu.cpp"
    "Source/UI/ContextMenu/VoxelAssetContextMenu.h"
    "Source/UI/ContextMenu/VoxelAssetContextMenu.cpp"
    "Source/UI/ContextMenu/ProgressAssetContextMenu.h"
    "Source/UI/ContextMenu/ProgressAssetContextMenu.cpp"
    "Source/UI/ContextMenu/TextureAssetContextMenu.h"
    "Source/UI/ContextMenu/TextureAssetContextMenu.cpp"
    "Source/UI/ContextMenu/VectorGraphicsAssetContextMenu.h"
    "Source/UI/ContextMenu/VectorGraphicsAssetContextMenu.cpp"
    "Source/UI/ContextMenu/AssetContextMenuFactory.h"
    "Source/UI/ContextMenu/AssetContextMenuFactory.cpp"
    "Source/UI/ContextMenu/AssetContextMenuRenderer.h"
    "Source/UI/ContextMenu/AssetContextMenuRenderer.cpp"
)
source_group("Source/UI/ContextMenu" FILES ${UI_ContextMenu_Files})

set(UI_Gizmo_Files
    "Source/UI/Gizmo/GizmoMode.h"
    "Source/UI/Gizmo/GizmoOverlay.h"
    "Source/UI/Gizmo/GizmoOverlay.cpp"
    "Source/UI/Gizmo/AssetGizmoController.h"
    "Source/UI/Gizmo/AssetGizmoController.cpp"
)
source_group("Source/UI/Gizmo" FILES ${UI_Gizmo_Files})

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

set(Picking_Files
    "Source/Picking/ViewportPickingController.h"
    "Source/Picking/ViewportPickingController.cpp"
)
source_group("Source/Picking" FILES ${Picking_Files})

set(Commands_Files
    "Source/Commands/CaptureScreenshotCommand.h"
    "Source/Commands/CaptureScreenshotCommand.cpp"
)
source_group("Source/Commands" FILES ${Commands_Files})

set(ALL_FILES
    ${Source_Files}
    ${Commands_Files}
    ${MCP_Files}
    ${Camera_Files}
    ${Picking_Files}
    ${Rendering_Files}
    ${UI_Menu_Files}
    ${UI_Component_Files}
    ${UI_ViewCube_Files}
    ${UI_ContextMenu_Files}
    ${UI_Gizmo_Files}
    ${UI_Project_Files}
    ${Shader_Files}
    ${GENERATED_SHADER_HEADERS}
)

add_executable(CadaverExe WIN32 ${ALL_FILES})
set_property(TARGET CadaverExe PROPERTY CXX_STANDARD 20)

target_link_libraries(CadaverExe
    glm::glm
    nlohmann_json::nlohmann_json
    CADacombRender
    CadaverLibrary
    CadaverUI
    Vulcant::Vulcant
    VulcantToolkit
    imgui::imgui
    imguizmo::imguizmo
    glfw
)

target_include_directories(CadaverExe PUBLIC 
    ./Source/
    ${SHADER_GEN_DIR}/
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
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_FILE_DIR:Polyglot>/python"
        "$<TARGET_FILE_DIR:CadaverExe>/python"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_FILE_DIR:Polyglot>/lua"
        "$<TARGET_FILE_DIR:CadaverExe>/lua"
)

set_property(TARGET CadaverExe PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:CadaverExe>")
set_property(TARGET CadaverExe PROPERTY VS_DEBUGGER_ENVIRONMENT 
    "PATH=$<IF:$<CONFIG:Debug>,${CMAKE_CURRENT_SOURCE_DIR}/../out/vcpkg_installed/x64-windows/debug/bin,${CMAKE_CURRENT_SOURCE_DIR}/../out/vcpkg_installed/x64-windows/bin>;$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug/bin,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin>;%PATH%"
)
