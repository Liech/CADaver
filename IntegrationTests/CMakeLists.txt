find_package(Catch2 REQUIRED)

set(Script_Python_Files
    "Scripts/Python/test_clear_project.py"
    "Scripts/Python/test_generate_landscape.py"
)
source_group("Scripts/Python" FILES ${Script_Python_Files})

set(Script_Lua_Files
    "Scripts/Lua/test_lua_pipeline.lua"
)
source_group("Scripts/Lua" FILES ${Script_Lua_Files})

set(Source_Files
    "Source/testMain.cpp"
    "Source/IntegrationTestRunner.h"
    "Source/IntegrationTestRunner.cpp"
)
source_group("Source" FILES ${Source_Files})

add_executable(CadaverIntegrationTest
    ${Source_Files}
    ${Script_Python_Files}
    ${Script_Lua_Files}
)

set_property(TARGET CadaverIntegrationTest PROPERTY CXX_STANDARD 20)
set_property(TARGET CadaverIntegrationTest PROPERTY COMPILE_DEFINITIONS 
    ISTESTPROJECT=1
    INTEGRATION_TEST_SCRIPTS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Scripts"
)

target_link_libraries(CadaverIntegrationTest PRIVATE
    Catch2::Catch2
)

target_include_directories(CadaverIntegrationTest PUBLIC
    ./Source/
)

set_target_properties(CadaverIntegrationTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/CADaver/CADaver"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:CadaverExe>"
    FOLDER "Tests"
)

set_property(TARGET CadaverIntegrationTest PROPERTY VS_DEBUGGER_ENVIRONMENT 
    "PATH=$<IF:$<CONFIG:Debug>,${CMAKE_CURRENT_SOURCE_DIR}/../../out/vcpkg_installed/x64-windows/debug/bin,${CMAKE_CURRENT_SOURCE_DIR}/../../out/vcpkg_installed/x64-windows/bin>;$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug/bin,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin>;%PATH%"
)

add_dependencies(CadaverIntegrationTest CadaverExe)


include(CTest)
include(Catch)
catch_discover_tests(CadaverIntegrationTest
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:CadaverExe>"
)
