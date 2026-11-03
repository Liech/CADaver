cmake_minimum_required(VERSION 3.20...3.30)

find_package(Catch2 REQUIRED)

project(CADaverUI)
project(CADaverUITest)

find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(imgui REQUIRED)
find_package(nfd CONFIG REQUIRED)

set(Source_Files
    "Source/CADaverUI/testMain.cpp"
)
source_group("Source" FILES ${Source_Files})

set(Component_Files
    "Source/CADaverUI/Component/AssetComboBox.h"
    "Source/CADaverUI/Component/AssetComboBox.cpp"
    "Source/CADaverUI/Component/LoadFileDialog.h"
    "Source/CADaverUI/Component/LoadFileDialog.cpp"
    "Source/CADaverUI/Component/SaveFileDialog.h"
    "Source/CADaverUI/Component/SaveFileDialog.cpp"
    "Source/CADaverUI/Component/VoxelResolutionComboBox.h"
    "Source/CADaverUI/Component/VoxelResolutionComboBox.cpp"
)
source_group("Source/Component" FILES ${Component_Files})

set(Windows_Files
    "Source/CADaverUI/Windows/Window.h"
    "Source/CADaverUI/Windows/Window.cpp"
    "Source/CADaverUI/Windows/Windows.h"
    "Source/CADaverUI/Windows/Windows.cpp"
    "Source/CADaverUI/Windows/WindowFactory.h"
    "Source/CADaverUI/Windows/WindowFactory.cpp"
)
source_group("Source/Windows" FILES ${Windows_Files})

set(Windows_About_Files
    "Source/CADaverUI/Windows/About/AboutWindow.h"
    "Source/CADaverUI/Windows/About/AboutWindow.cpp"
    "Source/CADaverUI/Windows/About/CommandsInfoWindow.h"
    "Source/CADaverUI/Windows/About/CommandsInfoWindow.cpp"
    "Source/CADaverUI/Windows/About/ConceptsInfoWindow.h"
    "Source/CADaverUI/Windows/About/ConceptsInfoWindow.cpp"
    "Source/CADaverUI/Windows/About/WindowsInfoWindow.h"
    "Source/CADaverUI/Windows/About/WindowsInfoWindow.cpp"
)
source_group("Source/Windows/About" FILES ${Windows_About_Files})

set(Windows_Conversion_Files
    "Source/CADaverUI/Windows/Conversion/CadToStlConversionModal.h"
    "Source/CADaverUI/Windows/Conversion/CadToStlConversionModal.cpp"
    "Source/CADaverUI/Windows/Conversion/CadToSvoConversionModal.h"
    "Source/CADaverUI/Windows/Conversion/CadToSvoConversionModal.cpp"
    "Source/CADaverUI/Windows/Conversion/StlToSvoConversionModal.h"
    "Source/CADaverUI/Windows/Conversion/StlToSvoConversionModal.cpp"
    "Source/CADaverUI/Windows/Conversion/SvoToStlConversionModal.h"
    "Source/CADaverUI/Windows/Conversion/SvoToStlConversionModal.cpp"
)
source_group("Source/Windows/Conversion" FILES ${Windows_Conversion_Files})

set(Windows_Generator_Files
    "Source/CADaverUI/Windows/Generator/LandscapeGeneratorWindow.h"
    "Source/CADaverUI/Windows/Generator/LandscapeGeneratorWindow.cpp"
)
source_group("Source/Windows/Generator" FILES ${Windows_Generator_Files})

set(Windows_IO_Files
    "Source/CADaverUI/Windows/IO/LoadStepAssetWindow.h"
    "Source/CADaverUI/Windows/IO/LoadStepAssetWindow.cpp"
    "Source/CADaverUI/Windows/IO/LoadStlAssetWindow.h"
    "Source/CADaverUI/Windows/IO/LoadStlAssetWindow.cpp"
    "Source/CADaverUI/Windows/IO/LoadSvoAssetWindow.h"
    "Source/CADaverUI/Windows/IO/LoadSvoAssetWindow.cpp"
    "Source/CADaverUI/Windows/IO/SaveStepAssetWindow.h"
    "Source/CADaverUI/Windows/IO/SaveStepAssetWindow.cpp"
    "Source/CADaverUI/Windows/IO/SaveStlAssetWindow.h"
    "Source/CADaverUI/Windows/IO/SaveStlAssetWindow.cpp"
    "Source/CADaverUI/Windows/IO/SaveVoxelAssetWindow.h"
    "Source/CADaverUI/Windows/IO/SaveVoxelAssetWindow.cpp"
)
source_group("Source/Windows/IO" FILES ${Windows_IO_Files})

set(Windows_Preferences_Files
    "Source/CADaverUI/Windows/Preferences/PreferenceTab.h"
    "Source/CADaverUI/Windows/Preferences/PreferenceTab.cpp"
    "Source/CADaverUI/Windows/Preferences/PreferenceTabFactory.h"
    "Source/CADaverUI/Windows/Preferences/PreferenceTabFactory.cpp"
    "Source/CADaverUI/Windows/Preferences/PreferencesWindow.h"
    "Source/CADaverUI/Windows/Preferences/PreferencesWindow.cpp"
    "Source/CADaverUI/Windows/Preferences/UIPreferencesTab.h"
    "Source/CADaverUI/Windows/Preferences/UIPreferencesTab.cpp"
    "Source/CADaverUI/Windows/Preferences/CameraPreferencesTab.h"
    "Source/CADaverUI/Windows/Preferences/CameraPreferencesTab.cpp"
    "Source/CADaverUI/Windows/Preferences/RenderPreferencesTab.h"
    "Source/CADaverUI/Windows/Preferences/RenderPreferencesTab.cpp"
    "Source/CADaverUI/Windows/Preferences/SVOPreferencesTab.h"
    "Source/CADaverUI/Windows/Preferences/SVOPreferencesTab.cpp"
)
source_group("Source/Windows/Preferences" FILES ${Windows_Preferences_Files})

set(Windows_Asset_Files
    "Source/CADaverUI/Windows/Asset/TextNoteEditWindow.h"
    "Source/CADaverUI/Windows/Asset/TextNoteEditWindow.cpp"
)
source_group("Source/Windows/Asset" FILES ${Windows_Asset_Files})

set(Windows_Script_Files
    "Source/CADaverUI/Windows/Script/ScriptWindow.h"
    "Source/CADaverUI/Windows/Script/ScriptWindow.cpp"
)
source_group("Source/Windows/Script" FILES ${Windows_Script_Files})

set(Windows_Console_Files
    "Source/CADaverUI/Windows/Console/ConsoleStreamBuffer.h"
    "Source/CADaverUI/Windows/Console/ConsoleStreamBuffer.cpp"
    "Source/CADaverUI/Windows/Console/ConsoleWindow.h"
    "Source/CADaverUI/Windows/Console/ConsoleWindow.cpp"
)
source_group("Source/Windows/Console" FILES ${Windows_Console_Files})

set(Windows_Project_Files
    "Source/CADaverUI/Windows/Project/ClearProjectModal.h"
    "Source/CADaverUI/Windows/Project/ClearProjectModal.cpp"
    "Source/CADaverUI/Windows/Project/RemoveAssetModal.h"
    "Source/CADaverUI/Windows/Project/RemoveAssetModal.cpp"
    "Source/CADaverUI/Windows/Project/TaskNotificationModal.h"
    "Source/CADaverUI/Windows/Project/TaskNotificationModal.cpp"
)
source_group("Source/Windows/Project" FILES ${Windows_Project_Files})

set(UIElement_Files
    "Source/CADaverUI/UIElement/UIElement.h"
    "Source/CADaverUI/UIElement/UIElement.cpp"
    "Source/CADaverUI/UIElement/NullUIElement.h"
    "Source/CADaverUI/UIElement/NullUIElement.cpp"
    "Source/CADaverUI/UIElement/UIElementFactory.h"
    "Source/CADaverUI/UIElement/UIElementFactory.cpp"
)
source_group("Source/UIElement" FILES ${UIElement_Files})

set(UIElement_Menu_Files
    "Source/CADaverUI/UIElement/Menu/MenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/MenuEntry.cpp"
    "Source/CADaverUI/UIElement/Menu/DynamicMenuNode.h"
    "Source/CADaverUI/UIElement/Menu/DynamicMenuNode.cpp"
    "Source/CADaverUI/UIElement/Menu/DynamicMenuRenderer.h"
    "Source/CADaverUI/UIElement/Menu/DynamicMenuRenderer.cpp"
)
source_group("Source/UIElement/Menu" FILES ${UIElement_Menu_Files})

set(UIElement_Menu_File_Files
    "Source/CADaverUI/UIElement/Menu/File/LoadStepMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/File/LoadStepMenuEntry.cpp"
    "Source/CADaverUI/UIElement/Menu/File/LoadStlMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/File/LoadStlMenuEntry.cpp"
    "Source/CADaverUI/UIElement/Menu/File/LoadSvoMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/File/LoadSvoMenuEntry.cpp"
    "Source/CADaverUI/UIElement/Menu/File/QuitMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/File/QuitMenuEntry.cpp"
)
source_group("Source/UIElement/Menu/File" FILES ${UIElement_Menu_File_Files})

set(UIElement_Menu_Edit_Files
    "Source/CADaverUI/UIElement/Menu/Edit/UndoMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/Edit/UndoMenuEntry.cpp"
    "Source/CADaverUI/UIElement/Menu/Edit/RedoMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/Edit/RedoMenuEntry.cpp"
    "Source/CADaverUI/UIElement/Menu/Edit/PreferencesMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/Edit/PreferencesMenuEntry.cpp"
)
source_group("Source/UIElement/Menu/Edit" FILES ${UIElement_Menu_Edit_Files})

set(UIElement_Menu_Create_Files
    "Source/CADaverUI/UIElement/Menu/Create/LandscapeMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/Create/LandscapeMenuEntry.cpp"
)
source_group("Source/UIElement/Menu/Create" FILES ${UIElement_Menu_Create_Files})

set(UIElement_Menu_Help_Files
    "Source/CADaverUI/UIElement/Menu/Help/AboutMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/Help/AboutMenuEntry.cpp"
    "Source/CADaverUI/UIElement/Menu/Help/CommandsInfoMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/Help/CommandsInfoMenuEntry.cpp"
    "Source/CADaverUI/UIElement/Menu/Help/ConceptsInfoMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/Help/ConceptsInfoMenuEntry.cpp"
    "Source/CADaverUI/UIElement/Menu/Help/WindowsInfoMenuEntry.h"
    "Source/CADaverUI/UIElement/Menu/Help/WindowsInfoMenuEntry.cpp"
)
source_group("Source/UIElement/Menu/Help" FILES ${UIElement_Menu_Help_Files})

set(UIElement_Radio_Files
    "Source/CADaverUI/UIElement/Radio/RadioUIElement.h"
    "Source/CADaverUI/UIElement/Radio/RadioUIElement.cpp"
    "Source/CADaverUI/UIElement/Radio/WireframeRadioElement.h"
    "Source/CADaverUI/UIElement/Radio/WireframeRadioElement.cpp"
    "Source/CADaverUI/UIElement/Radio/CullModeRadioElement.h"
    "Source/CADaverUI/UIElement/Radio/CullModeRadioElement.cpp"
    "Source/CADaverUI/UIElement/Radio/CameraModeRadioElement.h"
    "Source/CADaverUI/UIElement/Radio/CameraModeRadioElement.cpp"
    "Source/CADaverUI/UIElement/Radio/ProjectionRadioElement.h"
    "Source/CADaverUI/UIElement/Radio/ProjectionRadioElement.cpp"
    "Source/CADaverUI/UIElement/Radio/ColorThemeRadioElement.h"
    "Source/CADaverUI/UIElement/Radio/ColorThemeRadioElement.cpp"
)
source_group("Source/UIElement/Radio" FILES ${UIElement_Radio_Files})

set(Search_Files
    "Source/CADaverUI/Search/UIElementSearcher.h"
    "Source/CADaverUI/Search/UIElementSearcher.cpp"
    "Source/CADaverUI/Search/WindowSearcher.h"
    "Source/CADaverUI/Search/WindowSearcher.cpp"
    "Source/CADaverUI/Search/PreferenceTabSearcher.h"
    "Source/CADaverUI/Search/PreferenceTabSearcher.cpp"
    "Source/CADaverUI/Search/SearchBar.h"
    "Source/CADaverUI/Search/SearchBar.cpp"
)
source_group("Source/Search" FILES ${Search_Files})

set(Commands_Files
    "Source/CADaverUI/Commands/ClickUIElementCommand.h"
    "Source/CADaverUI/Commands/ClickUIElementCommand.cpp"
)
source_group("Source/Commands" FILES ${Commands_Files})

set(ALL_FILES
    ${Source_Files}
    ${Component_Files}
    ${Windows_Files}
    ${Windows_About_Files}
    ${Windows_Conversion_Files}
    ${Windows_Generator_Files}
    ${Windows_IO_Files}
    ${Windows_Preferences_Files}
    ${Windows_Asset_Files}
    ${Windows_Script_Files}
    ${Windows_Console_Files}
    ${Windows_Project_Files}
    ${UIElement_Files}
    ${UIElement_Menu_Files}
    ${UIElement_Menu_File_Files}
    ${UIElement_Menu_Edit_Files}
    ${UIElement_Menu_Create_Files}
    ${UIElement_Menu_Help_Files}
    ${UIElement_Radio_Files}
    ${Search_Files}
    ${Commands_Files}
)

############################################################################
############################################################################
############################################################################

add_executable(CadaverUITest ${ALL_FILES})
add_library(CadaverUI ${ALL_FILES})
set_property(TARGET CadaverUITest PROPERTY CXX_STANDARD 20)
set_property(TARGET CadaverUI PROPERTY CXX_STANDARD 20)

target_link_libraries(CadaverUITest PRIVATE
    CadaverLibrary
    Catch2::Catch2
    glm::glm
    nlohmann_json::nlohmann_json
    imgui::imgui
    nfd::nfd
)

target_link_libraries(CadaverUI PUBLIC
    CadaverLibrary
    glm::glm
    nlohmann_json::nlohmann_json
    imgui::imgui
    nfd::nfd
)

target_include_directories(CadaverUI PUBLIC
    ./Source/
    ./Source/CADaverUI/
)

target_include_directories(CadaverUITest PUBLIC
    ./Source/
    ./Source/CADaverUI/
)

set_property(TARGET CadaverUITest PROPERTY COMPILE_DEFINITIONS ISTESTPROJECT=1)

include(CTest)
include(Catch)
catch_discover_tests(CadaverUITest)
