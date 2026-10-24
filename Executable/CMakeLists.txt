cmake_minimum_required(VERSION 2.8...3.20)
project(CadaverExe)

############################################################################
############################################################################
############################################################################

find_package(nlohmann_json REQUIRED)
find_package(glm REQUIRED)

############################################################################
############################################################################
############################################################################

set(Source_Files
    "Source/main.cpp"
)
source_group("Source" FILES ${Source_Files})

set(ALL_FILES
    ${Source_Files}
)

############################################################################
############################################################################
############################################################################

add_executable(CadaverExe ${ALL_FILES})
set_property(TARGET CadaverExe PROPERTY CXX_STANDARD 20)

target_link_libraries(CadaverExe
glm::glm
nlohmann_json::nlohmann_json
CadaverLibrary
)

target_include_directories(CadaverExe PUBLIC 
./Source/
)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT CadaverExe)