find_package(Catch2 REQUIRED)

set(Source_Files
    "Source/testMain.cpp"
    "Source/SourceFile.h"
    "Source/SourceFile.cpp"
    "Source/SourceViolation.h"
    "Source/SourceViolation.cpp"
    "Source/SourceFileDiscoverer.h"
    "Source/SourceFileDiscoverer.cpp"
    "Source/ConstCastStaticTest.h"
    "Source/ConstCastStaticTest.cpp"
    "Source/ReinterpretCastStaticTest.h"
    "Source/ReinterpretCastStaticTest.cpp"
    "Source/FunctionPointerStaticTest.h"
    "Source/FunctionPointerStaticTest.cpp"
    "Source/Utf8BomStaticTest.h"
    "Source/Utf8BomStaticTest.cpp"
    "Source/PrefixDecrementStaticTest.h"
    "Source/PrefixDecrementStaticTest.cpp"
    "Source/DefaultKeywordStaticTest.h"
    "Source/DefaultKeywordStaticTest.cpp"
    "Source/TypeAliasStaticTest.h"
    "Source/TypeAliasStaticTest.cpp"
    "Source/CatchSectionStaticTest.h"
    "Source/CatchSectionStaticTest.cpp"
)
source_group("Source" FILES ${Source_Files})

add_executable(CadaverStaticAnalysisTest
    ${Source_Files}
)

set_property(TARGET CadaverStaticAnalysisTest PROPERTY CXX_STANDARD 20)
set_property(TARGET CadaverStaticAnalysisTest PROPERTY COMPILE_DEFINITIONS 
    ISTESTPROJECT=1
    STATIC_ANALYSIS_TARGET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(CadaverStaticAnalysisTest PRIVATE
    Catch2::Catch2
)

target_include_directories(CadaverStaticAnalysisTest PUBLIC
    ./Source/
)

set_target_properties(CadaverStaticAnalysisTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/CADaver/CADaver"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:CadaverExe>"
    FOLDER "Tests"
)

include(CTest)
include(Catch)
catch_discover_tests(CadaverStaticAnalysisTest
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:CadaverExe>"
)
