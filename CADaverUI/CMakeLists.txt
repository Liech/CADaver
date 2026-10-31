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

set(Windows_Project_Files
    "Source/CADaverUI/Windows/Project/ClearProjectModal.h"
    "Source/CADaverUI/Windows/Project/ClearProjectModal.cpp"
    "Source/CADaverUI/Windows/Project/RemoveAssetModal.h"
    "Source/CADaverUI/Windows/Project/RemoveAssetModal.cpp"
    "Source/CADaverUI/Windows/Project/TaskNotificationModal.h"
    "Source/CADaverUI/Windows/Project/TaskNotificationModal.cpp"
)
source_group("Source/Windows/Project" FILES ${Windows_Project_Files})

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
    ${Windows_Project_Files}
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
