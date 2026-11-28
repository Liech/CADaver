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
    "Source/CADaverLibrary/Commands/Conversion/ConvertClusteringToCadCommand.h"
    "Source/CADaverLibrary/Commands/Conversion/ConvertClusteringToCadCommand.cpp"
    "Source/CADaverLibrary/Commands/Conversion/ConvertStlToCadCommand.h"
    "Source/CADaverLibrary/Commands/Conversion/ConvertStlToCadCommand.cpp"
    "Source/CADaverLibrary/Commands/Conversion/ConvertStlToSvoCommand.h"
    "Source/CADaverLibrary/Commands/Conversion/ConvertStlToSvoCommand.cpp"
    "Source/CADaverLibrary/Commands/Conversion/ConvertSvoToStlCommand.h"
    "Source/CADaverLibrary/Commands/Conversion/ConvertSvoToStlCommand.cpp"
    "Source/CADaverLibrary/Commands/Conversion/ConvertSvoToStlMarchingCubesCommand.h"
    "Source/CADaverLibrary/Commands/Conversion/ConvertSvoToStlMarchingCubesCommand.cpp"
    "Source/CADaverLibrary/Commands/Conversion/ConvertSvoToStlSurfaceNetsCommand.h"
    "Source/CADaverLibrary/Commands/Conversion/ConvertSvoToStlSurfaceNetsCommand.cpp"
)
source_group("Source/Commands/Conversion" FILES ${Command_Conversion_Files})

set(Command_Generator_Files
    "Source/CADaverLibrary/Commands/Generator/GenerateBaseShapeCommand.h"
    "Source/CADaverLibrary/Commands/Generator/GenerateBaseShapeCommand.cpp"
    "Source/CADaverLibrary/Commands/Generator/GenerateFunctionSdfCommand.h"
    "Source/CADaverLibrary/Commands/Generator/GenerateFunctionSdfCommand.cpp"
    "Source/CADaverLibrary/Commands/Generator/GenerateFunctionSvoCommand.h"
    "Source/CADaverLibrary/Commands/Generator/GenerateFunctionSvoCommand.cpp"
    "Source/CADaverLibrary/Commands/Generator/GenerateLandscapeCommand.h"
    "Source/CADaverLibrary/Commands/Generator/GenerateLandscapeCommand.cpp"
)
source_group("Source/Commands/Generator" FILES ${Command_Generator_Files})

set(Command_IO_Files
    "Source/CADaverLibrary/Commands/IO/LoadAssetCommand.h"
    "Source/CADaverLibrary/Commands/IO/LoadAssetCommand.cpp"
    "Source/CADaverLibrary/Commands/IO/SaveAssetCommand.h"
    "Source/CADaverLibrary/Commands/IO/SaveAssetCommand.cpp"
)
source_group("Source/Commands/IO" FILES ${Command_IO_Files})

set(IO_AssetLoader_Files
    "Source/CADaverLibrary/IO/AssetLoader/AssetLoader.h"
    "Source/CADaverLibrary/IO/AssetLoader/AssetLoader.cpp"
    "Source/CADaverLibrary/IO/AssetLoader/AssetLoaderFactory.h"
    "Source/CADaverLibrary/IO/AssetLoader/AssetLoaderFactory.cpp"
    "Source/CADaverLibrary/IO/AssetLoader/CadAssetLoader.h"
    "Source/CADaverLibrary/IO/AssetLoader/CadAssetLoader.cpp"
    "Source/CADaverLibrary/IO/AssetLoader/FbxAssetLoader.h"
    "Source/CADaverLibrary/IO/AssetLoader/FbxAssetLoader.cpp"
    "Source/CADaverLibrary/IO/AssetLoader/SdfAssetLoader.h"
    "Source/CADaverLibrary/IO/AssetLoader/SdfAssetLoader.cpp"
    "Source/CADaverLibrary/IO/AssetLoader/TextureAssetLoader.h"
    "Source/CADaverLibrary/IO/AssetLoader/TextureAssetLoader.cpp"
    "Source/CADaverLibrary/IO/AssetLoader/TriangulationAssetLoader.h"
    "Source/CADaverLibrary/IO/AssetLoader/TriangulationAssetLoader.cpp"
    "Source/CADaverLibrary/IO/AssetLoader/VectorGraphicsAssetLoader.h"
    "Source/CADaverLibrary/IO/AssetLoader/VectorGraphicsAssetLoader.cpp"
    "Source/CADaverLibrary/IO/AssetLoader/VoxelAssetLoader.h"
    "Source/CADaverLibrary/IO/AssetLoader/VoxelAssetLoader.cpp"
)
source_group("Source/IO/AssetLoader" FILES ${IO_AssetLoader_Files})

set(IO_AssetSaver_Files
    "Source/CADaverLibrary/IO/AssetSaver/AssetSaver.h"
    "Source/CADaverLibrary/IO/AssetSaver/AssetSaver.cpp"
    "Source/CADaverLibrary/IO/AssetSaver/AssetSaverFactory.h"
    "Source/CADaverLibrary/IO/AssetSaver/AssetSaverFactory.cpp"
    "Source/CADaverLibrary/IO/AssetSaver/CadAssetSaver.h"
    "Source/CADaverLibrary/IO/AssetSaver/CadAssetSaver.cpp"
    "Source/CADaverLibrary/IO/AssetSaver/SdfAssetSaver.h"
    "Source/CADaverLibrary/IO/AssetSaver/SdfAssetSaver.cpp"
    "Source/CADaverLibrary/IO/AssetSaver/TextureAssetSaver.h"
    "Source/CADaverLibrary/IO/AssetSaver/TextureAssetSaver.cpp"
    "Source/CADaverLibrary/IO/AssetSaver/TriangulationAssetSaver.h"
    "Source/CADaverLibrary/IO/AssetSaver/TriangulationAssetSaver.cpp"
    "Source/CADaverLibrary/IO/AssetSaver/VectorGraphicsAssetSaver.h"
    "Source/CADaverLibrary/IO/AssetSaver/VectorGraphicsAssetSaver.cpp"
    "Source/CADaverLibrary/IO/AssetSaver/VoxelAssetSaver.h"
    "Source/CADaverLibrary/IO/AssetSaver/VoxelAssetSaver.cpp"
)
source_group("Source/IO/AssetSaver" FILES ${IO_AssetSaver_Files})

set(Command_Project_Files
    "Source/CADaverLibrary/Commands/Project/ClearProjectCommand.h"
    "Source/CADaverLibrary/Commands/Project/ClearProjectCommand.cpp"
    "Source/CADaverLibrary/Commands/Project/RemoveAssetCommand.h"
    "Source/CADaverLibrary/Commands/Project/RemoveAssetCommand.cpp"
    "Source/CADaverLibrary/Commands/Project/GetAssetNamesCommand.h"
    "Source/CADaverLibrary/Commands/Project/GetAssetNamesCommand.cpp"
    "Source/CADaverLibrary/Commands/Project/GetAssetsCommand.h"
    "Source/CADaverLibrary/Commands/Project/GetAssetsCommand.cpp"
    "Source/CADaverLibrary/Commands/Project/GetAssetCountCommand.h"
    "Source/CADaverLibrary/Commands/Project/GetAssetCountCommand.cpp"
    "Source/CADaverLibrary/Commands/Project/GetAssetInfoCommand.h"
    "Source/CADaverLibrary/Commands/Project/GetAssetInfoCommand.cpp"
    "Source/CADaverLibrary/Commands/Project/GetAssetVisibilityCommand.h"
    "Source/CADaverLibrary/Commands/Project/GetAssetVisibilityCommand.cpp"
    "Source/CADaverLibrary/Commands/Project/SetAssetVisibilityCommand.h"
    "Source/CADaverLibrary/Commands/Project/SetAssetVisibilityCommand.cpp"
)
source_group("Source/Commands/Project" FILES ${Command_Project_Files})

set(Command_Component_Files
    "Source/CADaverLibrary/Commands/Component/AddComponentCommand.h"
    "Source/CADaverLibrary/Commands/Component/AddComponentCommand.cpp"
    "Source/CADaverLibrary/Commands/Component/AddTextureComponentCommand.h"
    "Source/CADaverLibrary/Commands/Component/AddTextureComponentCommand.cpp"
    "Source/CADaverLibrary/Commands/Component/GetComponentCommand.h"
    "Source/CADaverLibrary/Commands/Component/GetComponentCommand.cpp"
    "Source/CADaverLibrary/Commands/Component/UpdateComponentCommand.h"
    "Source/CADaverLibrary/Commands/Component/UpdateComponentCommand.cpp"
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

set(Command_Preferences_Query_Files
    "Source/CADaverLibrary/Commands/Preferences/Query/GetCameraPreferencesCommand.h"
    "Source/CADaverLibrary/Commands/Preferences/Query/GetCameraPreferencesCommand.cpp"
    "Source/CADaverLibrary/Commands/Preferences/Query/GetRenderPreferencesCommand.h"
    "Source/CADaverLibrary/Commands/Preferences/Query/GetRenderPreferencesCommand.cpp"
    "Source/CADaverLibrary/Commands/Preferences/Query/GetSVOPreferencesCommand.h"
    "Source/CADaverLibrary/Commands/Preferences/Query/GetSVOPreferencesCommand.cpp"
    "Source/CADaverLibrary/Commands/Preferences/Query/GetUIPreferencesCommand.h"
    "Source/CADaverLibrary/Commands/Preferences/Query/GetUIPreferencesCommand.cpp"
)
source_group("Source/Commands/Preferences/Query" FILES ${Command_Preferences_Query_Files})

set(Command_Task_Files
    "Source/CADaverLibrary/Commands/Task/GetActiveTasksCommand.h"
    "Source/CADaverLibrary/Commands/Task/GetActiveTasksCommand.cpp"
    "Source/CADaverLibrary/Commands/Task/GetTaskStateCommand.h"
    "Source/CADaverLibrary/Commands/Task/GetTaskStateCommand.cpp"
    "Source/CADaverLibrary/Commands/Task/CancelTaskCommand.h"
    "Source/CADaverLibrary/Commands/Task/CancelTaskCommand.cpp"
)
source_group("Source/Commands/Task" FILES ${Command_Task_Files})

set(Command_Geometry_Files
    "Source/CADaverLibrary/Commands/Geometry/CalculateConnectedComponentsCommand.h"
    "Source/CADaverLibrary/Commands/Geometry/CalculateConnectedComponentsCommand.cpp"
    "Source/CADaverLibrary/Commands/Geometry/ClusterByNormalCommand.h"
    "Source/CADaverLibrary/Commands/Geometry/ClusterByNormalCommand.cpp"
    "Source/CADaverLibrary/Commands/Geometry/SimplifyMeshCommand.h"
    "Source/CADaverLibrary/Commands/Geometry/SimplifyMeshCommand.cpp"
    "Source/CADaverLibrary/Commands/Geometry/SplitClustersToAssetsCommand.h"
    "Source/CADaverLibrary/Commands/Geometry/SplitClustersToAssetsCommand.cpp"
    "Source/CADaverLibrary/Commands/Geometry/GetAssetAABBCommand.h"
    "Source/CADaverLibrary/Commands/Geometry/GetAssetAABBCommand.cpp"
    "Source/CADaverLibrary/Commands/Geometry/GetProjectAABBCommand.h"
    "Source/CADaverLibrary/Commands/Geometry/GetProjectAABBCommand.cpp"
)
source_group("Source/Commands/Geometry" FILES ${Command_Geometry_Files})

set(Command_Selection_Files
    "Source/CADaverLibrary/Commands/Selection/ClearSelectionCommand.h"
    "Source/CADaverLibrary/Commands/Selection/ClearSelectionCommand.cpp"
    "Source/CADaverLibrary/Commands/Selection/GetAssetSelectionCommand.h"
    "Source/CADaverLibrary/Commands/Selection/GetAssetSelectionCommand.cpp"
    "Source/CADaverLibrary/Commands/Selection/GetSelectedAssetsCommand.h"
    "Source/CADaverLibrary/Commands/Selection/GetSelectedAssetsCommand.cpp"
    "Source/CADaverLibrary/Commands/Selection/PickAssetCommand.h"
    "Source/CADaverLibrary/Commands/Selection/PickAssetCommand.cpp"
    "Source/CADaverLibrary/Commands/Selection/SelectAssetCommand.h"
    "Source/CADaverLibrary/Commands/Selection/SelectAssetCommand.cpp"
)
source_group("Source/Commands/Selection" FILES ${Command_Selection_Files})

set(Picking_Files
    "Source/CADaverLibrary/Picking/AssetPicker.h"
    "Source/CADaverLibrary/Picking/AssetPicker.cpp"
    "Source/CADaverLibrary/Picking/PickResult.h"
    "Source/CADaverLibrary/Picking/PickResult.cpp"
)
source_group("Source/Picking" FILES ${Picking_Files})

set(Picking_AssetRayPicker_Files
    "Source/CADaverLibrary/Picking/AssetRayPicker/AssetRayPicker.h"
    "Source/CADaverLibrary/Picking/AssetRayPicker/AssetRayPicker.cpp"
    "Source/CADaverLibrary/Picking/AssetRayPicker/AssetRayPickerFactory.h"
    "Source/CADaverLibrary/Picking/AssetRayPicker/AssetRayPickerFactory.cpp"
    "Source/CADaverLibrary/Picking/AssetRayPicker/CadAssetRayPicker.h"
    "Source/CADaverLibrary/Picking/AssetRayPicker/CadAssetRayPicker.cpp"
    "Source/CADaverLibrary/Picking/AssetRayPicker/SdfAssetRayPicker.h"
    "Source/CADaverLibrary/Picking/AssetRayPicker/SdfAssetRayPicker.cpp"
    "Source/CADaverLibrary/Picking/AssetRayPicker/TextureAssetRayPicker.h"
    "Source/CADaverLibrary/Picking/AssetRayPicker/TextureAssetRayPicker.cpp"
    "Source/CADaverLibrary/Picking/AssetRayPicker/VectorGraphicsAssetRayPicker.h"
    "Source/CADaverLibrary/Picking/AssetRayPicker/VectorGraphicsAssetRayPicker.cpp"
    "Source/CADaverLibrary/Picking/AssetRayPicker/TriangulationAssetRayPicker.h"
    "Source/CADaverLibrary/Picking/AssetRayPicker/TriangulationAssetRayPicker.cpp"
    "Source/CADaverLibrary/Picking/AssetRayPicker/TriangulationRayPicker.h"
    "Source/CADaverLibrary/Picking/AssetRayPicker/TriangulationRayPicker.cpp"
    "Source/CADaverLibrary/Picking/AssetRayPicker/VoxelAssetRayPicker.h"
    "Source/CADaverLibrary/Picking/AssetRayPicker/VoxelAssetRayPicker.cpp"
)
source_group("Source/Picking/AssetRayPicker" FILES ${Picking_AssetRayPicker_Files})

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

set(Command_Time_Files
    "Source/CADaverLibrary/Commands/Time/GetCurrentTimeCommand.h"
    "Source/CADaverLibrary/Commands/Time/GetCurrentTimeCommand.cpp"
    "Source/CADaverLibrary/Commands/Time/StepTimeCommand.h"
    "Source/CADaverLibrary/Commands/Time/StepTimeCommand.cpp"
)
source_group("Source/Commands/Time" FILES ${Command_Time_Files})

set(Database_Files
    "Source/CADaverLibrary/Database/Database.h"
    "Source/CADaverLibrary/Database/Database.cpp"
    "Source/CADaverLibrary/Database/Project.h"
    "Source/CADaverLibrary/Database/Project.cpp"
    "Source/CADaverLibrary/Database/Asset.h"
    "Source/CADaverLibrary/Database/Asset.cpp"
    "Source/CADaverLibrary/Database/AssetComponent.h"
    "Source/CADaverLibrary/Database/AssetComponent.cpp"
    "Source/CADaverLibrary/Database/Component/ComponentFactory.h"
    "Source/CADaverLibrary/Database/Component/ComponentFactory.cpp"
    "Source/CADaverLibrary/Database/Component/FaceValuesComponent.h"
    "Source/CADaverLibrary/Database/Component/FaceValuesComponent.cpp"
    "Source/CADaverLibrary/Database/Component/RigidBodyComponent.h"
    "Source/CADaverLibrary/Database/Component/RigidBodyComponent.cpp"
    "Source/CADaverLibrary/Database/Component/SoftBodyComponent.h"
    "Source/CADaverLibrary/Database/Component/SoftBodyComponent.cpp"
    "Source/CADaverLibrary/Database/Component/TextNoteComponent.h"
    "Source/CADaverLibrary/Database/Component/TextNoteComponent.cpp"
    "Source/CADaverLibrary/Database/Component/AnimationComponent.h"
    "Source/CADaverLibrary/Database/Component/AnimationComponent.cpp"
    "Source/CADaverLibrary/Database/Component/SkeletonComponent.h"
    "Source/CADaverLibrary/Database/Component/SkeletonComponent.cpp"
    "Source/CADaverLibrary/Database/Component/TextureComponent.h"
    "Source/CADaverLibrary/Database/Component/TextureComponent.cpp"
    "Source/CADaverLibrary/Database/Component/TriangulationSkinningComponent.h"
    "Source/CADaverLibrary/Database/Component/TriangulationSkinningComponent.cpp"
    "Source/CADaverLibrary/Database/Component/UVComponent.h"
    "Source/CADaverLibrary/Database/Component/UVComponent.cpp"
    "Source/CADaverLibrary/Database/Physics/PhysicsSimulation.h"
    "Source/CADaverLibrary/Database/Physics/PhysicsSimulation.cpp"
)
source_group("Source/Database" FILES ${Database_Files})

set(Database_Selection_Files
    "Source/CADaverLibrary/Database/Selection/Selection.h"
    "Source/CADaverLibrary/Database/Selection/Selection.cpp"
    "Source/CADaverLibrary/Database/Selection/SelectionEntry.h"
    "Source/CADaverLibrary/Database/Selection/SelectionEntry.cpp"
    "Source/CADaverLibrary/Database/Selection/SubObjectSelection.h"
    "Source/CADaverLibrary/Database/Selection/SubObjectSelection.cpp"
    "Source/CADaverLibrary/Database/Selection/SubObjectSelectionFactory.h"
    "Source/CADaverLibrary/Database/Selection/SubObjectSelectionFactory.cpp"
    "Source/CADaverLibrary/Database/Selection/TriangleSubObjectSelection.h"
    "Source/CADaverLibrary/Database/Selection/TriangleSubObjectSelection.cpp"
    "Source/CADaverLibrary/Database/Selection/VoxelSubObjectSelection.h"
    "Source/CADaverLibrary/Database/Selection/VoxelSubObjectSelection.cpp"
)
source_group("Source/Database/Selection" FILES ${Database_Selection_Files})

set(Execution_Files
    "Source/CADaverLibrary/Execution/Command.h"
    "Source/CADaverLibrary/Execution/Command.cpp"
    "Source/CADaverLibrary/Execution/CommandFactory.h"
    "Source/CADaverLibrary/Execution/CommandFactory.cpp"
    "Source/CADaverLibrary/Execution/Defer.h"
    "Source/CADaverLibrary/Execution/Defer.cpp"
    "Source/CADaverLibrary/Execution/ErrorResult.h"
    "Source/CADaverLibrary/Execution/ErrorResult.cpp"
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
    "Source/CADaverLibrary/Utility/LuaSVOField.h"
    "Source/CADaverLibrary/Utility/LuaSVOField.cpp"
    "Source/CADaverLibrary/Utility/MatrixJson.h"
    "Source/CADaverLibrary/Utility/MatrixJson.cpp"
    "Source/CADaverLibrary/Commands/Transform/TransformAssetCommand.h"
    "Source/CADaverLibrary/Commands/Transform/TransformAssetCommand.cpp"
    "Source/CADaverLibrary/Commands/Transform/GetTransformationAssetCommand.h"
    "Source/CADaverLibrary/Commands/Transform/GetTransformationAssetCommand.cpp"
)
source_group("Source/Utility" FILES ${Utility_Files})

set(Mcp_Files
    "Source/CADaverLibrary/MCP/McpPipeStream.h"
    "Source/CADaverLibrary/MCP/McpPipeStream.cpp"
    "Source/CADaverLibrary/MCP/McpServer.h"
    "Source/CADaverLibrary/MCP/McpServer.cpp"
    "Source/CADaverLibrary/MCP/McpSocketServer.h"
    "Source/CADaverLibrary/MCP/McpSocketServer.cpp"
)
source_group("Source/MCP" FILES ${Mcp_Files})

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
    "Source/CADaverLibrary/Concept/CommandLineConcept.h"
    "Source/CADaverLibrary/Concept/CommandLineConcept.cpp"
    "Source/CADaverLibrary/Concept/McpConcept.h"
    "Source/CADaverLibrary/Concept/McpConcept.cpp"
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
    "Source/CADaverLibrary/Database/Asset/SdfAsset.h"
    "Source/CADaverLibrary/Database/Asset/SdfAsset.cpp"
    "Source/CADaverLibrary/Database/Asset/TextureAsset.h"
    "Source/CADaverLibrary/Database/Asset/TextureAsset.cpp"
    "Source/CADaverLibrary/Database/Asset/VectorGraphicsAsset.h"
    "Source/CADaverLibrary/Database/Asset/VectorGraphicsAsset.cpp"
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
    ${IO_AssetLoader_Files}
    ${IO_AssetSaver_Files}
    ${Command_Project_Files}
    ${Command_Component_Files}
    ${Command_Preferences_Files}
    ${Command_Preferences_Query_Files}
    ${Command_Task_Files}
    ${Command_Geometry_Files}
    ${Command_Time_Files}
    ${Command_Selection_Files}
    ${Picking_Files}
    ${Picking_AssetRayPicker_Files}
    ${Command_Search_Files}
    ${Concept_Files}
    ${Search_Files}
    ${Database_Preferences_Files}
    ${Database_Files}
    ${Database_Selection_Files}
    ${Database_Asset_Files}
    ${Execution_Files}
    ${Utility_Files}
    ${Mcp_Files}
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

if(WIN32)
    target_link_libraries(CadaverLibrary PUBLIC ws2_32)
    target_link_libraries(CadaverLibraryTest PRIVATE ws2_32)
endif()

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
