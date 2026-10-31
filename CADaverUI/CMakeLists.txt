cmake_minimum_required(VERSION 3.20...3.30)

find_package(Catch2 REQUIRED)

project(CADaverUI)
project(CADaverUITest)

find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(imgui REQUIRED)

set(Source_Files
    "Source/CADaverUI/testMain.cpp"
)
source_group("Source" FILES ${Source_Files})

set(Component_Files
    "Source/CADaverUI/Component/AssetComboBox.h"
    "Source/CADaverUI/Component/AssetComboBox.cpp"
    "Source/CADaverUI/Component/VoxelResolutionComboBox.h"
    "Source/CADaverUI/Component/VoxelResolutionComboBox.cpp"
)
source_group("Source/Component" FILES ${Component_Files})

set(Windows_Files
    "Source/CADaverUI/Windows/Window.h"
    "Source/CADaverUI/Windows/Window.cpp"
    "Source/CADaverUI/Windows/WindowFactory.h"
    "Source/CADaverUI/Windows/WindowFactory.cpp"
)
source_group("Source/Windows" FILES ${Windows_Files})

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

set(ALL_FILES
    ${Source_Files}
    ${Component_Files}
    ${Windows_Files}
    ${Windows_Conversion_Files}
    ${Windows_Generator_Files}
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
)

target_link_libraries(CadaverUI PUBLIC
    CadaverLibrary
    glm::glm
    nlohmann_json::nlohmann_json
    imgui::imgui
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
