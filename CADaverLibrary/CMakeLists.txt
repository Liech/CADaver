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
    "Source/CADaverLibrary/Commands/Project/GetAssetNamesCommand.h"
    "Source/CADaverLibrary/Commands/Project/GetAssetNamesCommand.cpp"
    "Source/CADaverLibrary/Commands/Project/GetAssetCountCommand.h"
    "Source/CADaverLibrary/Commands/Project/GetAssetCountCommand.cpp"
    "Source/CADaverLibrary/Commands/Project/GetAssetInfoCommand.h"
    "Source/CADaverLibrary/Commands/Project/GetAssetInfoCommand.cpp"
)
source_group("Source/Commands/Project" FILES ${Command_Project_Files})

set(Command_Component_Files
    "Source/CADaverLibrary/Commands/Component/AddNoteCommand.h"
    "Source/CADaverLibrary/Commands/Component/AddNoteCommand.cpp"
    "Source/CADaverLibrary/Commands/Component/RemoveComponentCommand.h"
    "Source/CADaverLibrary/Commands/Component/RemoveComponentCommand.cpp"
)
source_group("Source/Commands/Component" FILES ${Command_Component_Files})

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
    "Source/CADaverLibrary/Database/AssetComponent.h"
    "Source/CADaverLibrary/Database/AssetComponent.cpp"
    "Source/CADaverLibrary/Database/Component/TextNoteComponent.h"
    "Source/CADaverLibrary/Database/Component/TextNoteComponent.cpp"
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
    "Source/CADaverLibrary/Execution/ScriptBridge.h"
    "Source/CADaverLibrary/Execution/ScriptBridge.cpp"
    "Source/CADaverLibrary/Execution/UndoContainer.h"
    "Source/CADaverLibrary/Execution/UndoContainer.cpp"
    "Source/CADaverLibrary/Execution/UndoStack.h"
    "Source/CADaverLibrary/Execution/UndoStack.cpp"
)
source_group("Source/Execution" FILES ${Execution_Files})

set(Utility_Files
    "Source/CADaverLibrary/Utility/IOValueGrabber.h"
    "Source/CADaverLibrary/Utility/IOValueGrabber.cpp"
)
source_group("Source/Utility" FILES ${Utility_Files})

set(Command_Search_Files
    "Source/CADaverLibrary/Commands/Search/GetDocumentationCommand.h"
    "Source/CADaverLibrary/Commands/Search/GetDocumentationCommand.cpp"
    "Source/CADaverLibrary/Commands/Search/SearchCommand.h"
    "Source/CADaverLibrary/Commands/Search/SearchCommand.cpp"
)
source_group("Source/Commands/Search" FILES ${Command_Search_Files})

set(Concept_Files
    "Source/CADaverLibrary/Concept/Concept.h"
    "Source/CADaverLibrary/Concept/Concept.cpp"
    "Source/CADaverLibrary/Concept/ConceptFactory.h"
    "Source/CADaverLibrary/Concept/ConceptFactory.cpp"
    "Source/CADaverLibrary/Concept/AssetConcept.h"
    "Source/CADaverLibrary/Concept/AssetConcept.cpp"
    "Source/CADaverLibrary/Concept/CommandConcept.h"
    "Source/CADaverLibrary/Concept/CommandConcept.cpp"
    "Source/CADaverLibrary/Concept/StepFileConcept.h"
    "Source/CADaverLibrary/Concept/StepFileConcept.cpp"
    "Source/CADaverLibrary/Concept/StlFileConcept.h"
    "Source/CADaverLibrary/Concept/StlFileConcept.cpp"
    "Source/CADaverLibrary/Concept/SubAssetConcept.h"
    "Source/CADaverLibrary/Concept/SubAssetConcept.cpp"
    "Source/CADaverLibrary/Concept/SvoFileConcept.h"
    "Source/CADaverLibrary/Concept/SvoFileConcept.cpp"
)
source_group("Source/Concept" FILES ${Concept_Files})

set(Search_Files
    "Source/CADaverLibrary/Search/AssetSearcher.h"
    "Source/CADaverLibrary/Search/AssetSearcher.cpp"
    "Source/CADaverLibrary/Search/CommandSearcher.h"
    "Source/CADaverLibrary/Search/CommandSearcher.cpp"
    "Source/CADaverLibrary/Search/ConceptSearcher.h"
    "Source/CADaverLibrary/Search/ConceptSearcher.cpp"
    "Source/CADaverLibrary/Search/SearchMatcher.h"
    "Source/CADaverLibrary/Search/SearchMatcher.cpp"
    "Source/CADaverLibrary/Search/SearchResult.h"
    "Source/CADaverLibrary/Search/SearchResult.cpp"
    "Source/CADaverLibrary/Search/SearchService.h"
    "Source/CADaverLibrary/Search/SearchService.cpp"
    "Source/CADaverLibrary/Search/Searcher.h"
    "Source/CADaverLibrary/Search/Searcher.cpp"
)
source_group("Source/Search" FILES ${Search_Files})

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
    ${Command_Component_Files}
    ${Command_Preferences_Files}
    ${Command_Search_Files}
    ${Concept_Files}
    ${Search_Files}
    ${Database_Preferences_Files}
    ${Database_Files}
    ${Database_Asset_Files}
    ${Execution_Files}
    ${Utility_Files}
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
    Polyglot
)

target_link_libraries(CadaverLibrary PUBLIC
    CADacomb::CADacomb
    glm::glm
    nlohmann_json::nlohmann_json
    Polyglot
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
set_property(TARGET CadaverLibraryTest PROPERTY FOLDER "Tests")

include(CTest)
include(Catch)
catch_discover_tests(CadaverLibraryTest)
