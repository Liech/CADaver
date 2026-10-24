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

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT CadaverExe)

add_custom_command(TARGET CadaverExe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<IF:$<CONFIG:Debug>,${OpenCASCADE_INSTALL_PREFIX}/debug/bin,${OpenCASCADE_INSTALL_PREFIX}/bin>"
        $<TARGET_FILE_DIR:CadaverExe>
)

set_property(TARGET CadaverExe PROPERTY VS_DEBUGGER_ENVIRONMENT 
    "PATH=$<IF:$<CONFIG:Debug>,${OpenCASCADE_INSTALL_PREFIX}/debug/bin,${OpenCASCADE_INSTALL_PREFIX}/bin>;%PATH%"
)