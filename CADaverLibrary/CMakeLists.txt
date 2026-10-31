cmake_minimum_required(VERSION 3.20...3.30)

find_package(Catch2 REQUIRED)

project(CADaverLibrary)
project(CADaverLibraryTest)

find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)

set(Source_Files
    "Source/CADaverLibrary/testMain.cpp"
)
source_group("Source" FILES ${Source_Files})

set(Conversion_Files
    "Source/CADaverLibrary/Conversion/AssetConverter.h"
    "Source/CADaverLibrary/Conversion/AssetConverter.cpp"
)
source_group("Source/Conversion" FILES ${Conversion_Files})

set(Database_Preferences_Files
    "Source/CADaverLibrary/Database/Preferences/Preferences.h"
    "Source/CADaverLibrary/Database/Preferences/Preferences.cpp"
    "Source/CADaverLibrary/Database/Preferences/UIPreferences.h"
    "Source/CADaverLibrary/Database/Preferences/UIPreferences.cpp"
    "Source/CADaverLibrary/Database/Preferences/CameraPreferences.h"
    "Source/CADaverLibrary/Database/Preferences/CameraPreferences.cpp"
    "Source/CADaverLibrary/Database/Preferences/RenderPreferences.h"
    "Source/CADaverLibrary/Database/Preferences/RenderPreferences.cpp"
    "Source/CADaverLibrary/Database/Preferences/SVOPreferences.h"
    "Source/CADaverLibrary/Database/Preferences/SVOPreferences.cpp"
)
source_group("Source/Database/Preferences" FILES ${Database_Preferences_Files})

set(Database_Files
    "Source/CADaverLibrary/Database/Database.h"
    "Source/CADaverLibrary/Database/Database.cpp"
    "Source/CADaverLibrary/Database/Project.h"
    "Source/CADaverLibrary/Database/Project.cpp"
    "Source/CADaverLibrary/Database/Asset.h"
    "Source/CADaverLibrary/Database/Asset.cpp"
)
source_group("Source/Database" FILES ${Database_Files})

set(Execution_Files
    "Source/CADaverLibrary/Execution/Command.h"
    "Source/CADaverLibrary/Execution/Command.cpp"
    "Source/CADaverLibrary/Execution/CommandFactory.h"
    "Source/CADaverLibrary/Execution/CommandFactory.cpp"
    "Source/CADaverLibrary/Execution/Defer.h"
    "Source/CADaverLibrary/Execution/Defer.cpp"
    "Source/CADaverLibrary/Execution/GatedDatabase.h"
    "Source/CADaverLibrary/Execution/GatedDatabase.cpp"
)
source_group("Source/Execution" FILES ${Execution_Files})

set(Undo_Files
    "Source/CADaverLibrary/Undo/UndoCommand.h"
    "Source/CADaverLibrary/Undo/UndoCommand.cpp"
    "Source/CADaverLibrary/Undo/RedoCommand.h"
    "Source/CADaverLibrary/Undo/RedoCommand.cpp"
    "Source/CADaverLibrary/Undo/UndoRecord.h"
    "Source/CADaverLibrary/Undo/UndoRecord.cpp"
    "Source/CADaverLibrary/Undo/AddAssetUndoRecord.h"
    "Source/CADaverLibrary/Undo/AddAssetUndoRecord.cpp"
    "Source/CADaverLibrary/Undo/RemoveAssetUndoRecord.h"
    "Source/CADaverLibrary/Undo/RemoveAssetUndoRecord.cpp"
    "Source/CADaverLibrary/Undo/ReplaceAssetUndoRecord.h"
    "Source/CADaverLibrary/Undo/ReplaceAssetUndoRecord.cpp"
    "Source/CADaverLibrary/Undo/ClearProjectUndoRecord.h"
    "Source/CADaverLibrary/Undo/ClearProjectUndoRecord.cpp"
    "Source/CADaverLibrary/Undo/PreferencesUndoRecord.h"
    "Source/CADaverLibrary/Undo/PreferencesUndoRecord.cpp"
    "Source/CADaverLibrary/Undo/UndoStack.h"
    "Source/CADaverLibrary/Undo/UndoStack.cpp"
)
source_group("Source/Undo" FILES ${Undo_Files})

set(Database_Asset_Files
    "Source/CADaverLibrary/Database/Asset/CadAsset.h"
    "Source/CADaverLibrary/Database/Asset/CadAsset.cpp"
    "Source/CADaverLibrary/Database/Asset/ProgressBarAsset.h"
    "Source/CADaverLibrary/Database/Asset/ProgressBarAsset.cpp"
    "Source/CADaverLibrary/Database/Asset/TriangulationAsset.h"
    "Source/CADaverLibrary/Database/Asset/TriangulationAsset.cpp"
    "Source/CADaverLibrary/Database/Asset/VoxelAsset.h"
    "Source/CADaverLibrary/Database/Asset/VoxelAsset.cpp"
)
source_group("Source/Database/Asset" FILES ${Database_Asset_Files})

set(ALL_FILES
    ${Source_Files}
    ${Conversion_Files}
    ${Database_Preferences_Files}
    ${Database_Files}
    ${Database_Asset_Files}
    ${Execution_Files}
    ${Undo_Files}
)

############################################################################
############################################################################
############################################################################

add_executable(CadaverLibraryTest ${ALL_FILES})
add_library(CadaverLibrary ${ALL_FILES})
set_property(TARGET CadaverLibraryTest PROPERTY CXX_STANDARD 20)
set_property(TARGET CadaverLibrary PROPERTY CXX_STANDARD 20)

target_link_libraries(CadaverLibraryTest PRIVATE
    CADacomb::CADacomb
    Catch2::Catch2
    glm::glm
    nlohmann_json::nlohmann_json
)

target_link_libraries(CadaverLibrary PUBLIC
    CADacomb::CADacomb
    glm::glm
    nlohmann_json::nlohmann_json
)

target_include_directories(CadaverLibrary PUBLIC
    ./Source/
    ./Source/CADaverLibrary/
)

target_include_directories(CadaverLibraryTest PUBLIC
    ./Source/
    ./Source/CADaverLibrary/
)

set_property(TARGET CadaverLibraryTest PROPERTY COMPILE_DEFINITIONS ISTESTPROJECT=1)

include(CTest)
include(Catch)
catch_discover_tests(CadaverLibraryTest)
