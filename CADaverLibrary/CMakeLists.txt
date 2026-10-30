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

set(ALL_FILES
    ${Source_Files}
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
)

target_include_directories(CadaverLibraryTest PUBLIC
    ./Source/
)

set_property(TARGET CadaverLibraryTest PROPERTY COMPILE_DEFINITIONS ISTESTPROJECT=1)

include(CTest)
include(Catch)
catch_discover_tests(CadaverLibraryTest)
