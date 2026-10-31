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

set(Command_Files
    "Source/CADaverLibrary/Commands/RULES.md"
)
source_group("Source/Commands" FILES ${Command_Files})

set(Command_Conversion_Files
    "Source/CADaverLibrary/Commands/Conversion/ConvertCadToStlCommand.h"
    "Source/CADaverLibrary/Commands/Conversion/ConvertCadToStlCommand.cpp"
    "Source/CADaverLibrary/Commands/Conversion/ConvertCadToSvoCommand.h"
    "Source/CADaverLibrary/Commands/Conversion/ConvertCadToSvoCommand.cpp"
    "Source/CADaverLibrary/Commands/Conversion/ConvertStlToSvoCommand.h"
    "Source/CADaverLibrary/Commands/Conversion/ConvertStlToSvoCommand.cpp"
    "Source/CADaverLibrary/Commands/Conversion/ConvertSvoToStlCommand.h"
    "Source/CADaverLibrary/Commands/Conversion/ConvertSvoToStlCommand.cpp"
)
source_group("Source/Commands/Conversion" FILES ${Command_Conversion_Files})

set(Command_Generator_Files
    "Source/CADaverLibrary/Commands/Generator/GenerateLandscapeCommand.h"
    "Source/CADaverLibrary/Commands/Generator/GenerateLandscapeCommand.cpp"
)
source_group("Source/Commands/Generator" FILES ${Command_Generator_Files})

set(Command_IO_Files
    "Source/CADaverLibrary/Commands/IO/LoadStepAssetCommand.h"
    "Source/CADaverLibrary/Commands/IO/LoadStepAssetCommand.cpp"
    "Source/CADaverLibrary/Commands/IO/LoadStlAssetCommand.h"
    "Source/CADaverLibrary/Commands/IO/LoadStlAssetCommand.cpp"
    "Source/CADaverLibrary/Commands/IO/LoadSvoAssetCommand.h"
    "Source/CADaverLibrary/Commands/IO/LoadSvoAssetCommand.cpp"
    "Source/CADaverLibrary/Commands/IO/SaveStepAssetCommand.h"
    "Source/CADaverLibrary/Commands/IO/SaveStepAssetCommand.cpp"
    "Source/CADaverLibrary/Commands/IO/SaveStlAssetCommand.h"
    "Source/CADaverLibrary/Commands/IO/SaveStlAssetCommand.cpp"
    "Source/CADaverLibrary/Commands/IO/SaveVoxelAssetCommand.h"
    "Source/CADaverLibrary/Commands/IO/SaveVoxelAssetCommand.cpp"
)
source_group("Source/Commands/IO" FILES ${Command_IO_Files})

set(Command_Project_Files
    "Source/CADaverLibrary/Commands/Project/ClearProjectCommand.h"
    "Source/CADaverLibrary/Commands/Project/ClearProjectCommand.cpp"
    "Source/CADaverLibrary/Commands/Project/RemoveAssetCommand.h"
    "Source/CADaverLibrary/Commands/Project/RemoveAssetCommand.cpp"
)
source_group("Source/Commands/Project" FILES ${Command_Project_Files})

set(Command_Preferences_Files
    "Source/CADaverLibrary/Commands/Preferences/UpdateCameraPreferencesCommand.h"
    "Source/CADaverLibrary/Commands/Preferences/UpdateCameraPreferencesCommand.cpp"
    "Source/CADaverLibrary/Commands/Preferences/UpdateUIPreferencesCommand.h"
    "Source/CADaverLibrary/Commands/Preferences/UpdateUIPreferencesCommand.cpp"
    "Source/CADaverLibrary/Commands/Preferences/UpdateRenderPreferencesCommand.h"
    "Source/CADaverLibrary/Commands/Preferences/UpdateRenderPreferencesCommand.cpp"
    "Source/CADaverLibrary/Commands/Preferences/UpdateSVOPreferencesCommand.h"
    "Source/CADaverLibrary/Commands/Preferences/UpdateSVOPreferencesCommand.cpp"
)
source_group("Source/Commands/Preferences" FILES ${Command_Preferences_Files})

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
    "Source/CADaverLibrary/Execution/UndoContainer.h"
    "Source/CADaverLibrary/Execution/UndoContainer.cpp"
    "Source/CADaverLibrary/Execution/UndoStack.h"
    "Source/CADaverLibrary/Execution/UndoStack.cpp"
)
source_group("Source/Execution" FILES ${Execution_Files})

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
    ${Command_Files}
    ${Command_Conversion_Files}
    ${Command_Generator_Files}
    ${Command_IO_Files}
    ${Command_Project_Files}
    ${Command_Preferences_Files}
    ${Database_Preferences_Files}
    ${Database_Files}
    ${Database_Asset_Files}
    ${Execution_Files}
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
