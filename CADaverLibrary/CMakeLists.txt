cmake_minimum_required(VERSION 3.20...3.30)

project(CADaverLibrary)
project(CADaverLibraryTest)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../out/vcpkg_installed/x64-windows/share/opencascade")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../out/vcpkg_installed/x64-windows")
endif()

find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(nfd CONFIG REQUIRED) 
find_package(OpenCASCADE)
include_directories(SYSTEM ${OpenCASCADE_INCLUDE_DIR})

set(Source_Files
    "Source/CADaverLibrary/testMain.cpp"
)
source_group("Source" FILES ${Source_Files})

set(CADShapes_Files
    "Source/CADaverLibrary/CAD/CADShape.cpp"
    "Source/CADaverLibrary/CAD/CADShape.h"
    "Source/CADaverLibrary/CAD/CADCompound.cpp"
    "Source/CADaverLibrary/CAD/CADCompound.h"
    "Source/CADaverLibrary/CAD/CADCompSolid.cpp"
    "Source/CADaverLibrary/CAD/CADCompSolid.h"
    "Source/CADaverLibrary/CAD/CADEdge.cpp"
    "Source/CADaverLibrary/CAD/CADEdge.h"
    "Source/CADaverLibrary/CAD/CADFace.cpp"
    "Source/CADaverLibrary/CAD/CADFace.h"
    "Source/CADaverLibrary/CAD/CADShell.cpp"
    "Source/CADaverLibrary/CAD/CADShell.h"
    "Source/CADaverLibrary/CAD/CADSolid.cpp"
    "Source/CADaverLibrary/CAD/CADSolid.h"
    "Source/CADaverLibrary/CAD/CADVertex.cpp"
    "Source/CADaverLibrary/CAD/CADVertex.h"
    "Source/CADaverLibrary/CAD/CADWire.cpp"
    "Source/CADaverLibrary/CAD/CADWire.h"
    "Source/CADaverLibrary/CAD/TopAbs2String.cpp"
    "Source/CADaverLibrary/CAD/TopAbs2String.h"
    "Source/CADaverLibrary/CAD/CADShapeFactory.cpp"
    "Source/CADaverLibrary/CAD/CADShapeFactory.h"
)
source_group("Source/CAD" FILES ${CADShapes_Files})

set(Util_Files
    "Source/CADaverLibrary/Util/SaveFileDialog.cpp"
    "Source/CADaverLibrary/Util/SaveFileDialog.h"
    "Source/CADaverLibrary/Util/LoadFileDialog.cpp"
    "Source/CADaverLibrary/Util/LoadFileDialog.h"
)
source_group("Source/Util" FILES ${Util_Files})

set(Operation_Files
    "Source/CADaverLibrary/Operation/ExtrudeOperation.cpp"
    "Source/CADaverLibrary/Operation/ExtrudeOperation.h"
    "Source/CADaverLibrary/Operation/TriangulateOperation.cpp"
    "Source/CADaverLibrary/Operation/TriangulateOperation.h"
)
source_group("Source/Operation" FILES ${Operation_Files})

set(OperationIO_Files
    "Source/CADaverLibrary/Operation/IO/SaveTriangulationOperation.cpp"
    "Source/CADaverLibrary/Operation/IO/SaveTriangulationOperation.h"
    "Source/CADaverLibrary/Operation/IO/LoadTriangulationOperation.cpp"
    "Source/CADaverLibrary/Operation/IO/LoadTriangulationOperation.h"
    "Source/CADaverLibrary/Operation/IO/LoadCADOperation.cpp"
    "Source/CADaverLibrary/Operation/IO/LoadCADOperation.h"
    "Source/CADaverLibrary/Operation/IO/SaveCADOperation.cpp"
    "Source/CADaverLibrary/Operation/IO/SaveCADOperation.h"
    "Source/CADaverLibrary/Operation/IO/LoadVoxelOperation.cpp"
    "Source/CADaverLibrary/Operation/IO/LoadVoxelOperation.h"
    "Source/CADaverLibrary/Operation/IO/SaveVoxelOperation.cpp"
    "Source/CADaverLibrary/Operation/IO/SaveVoxelOperation.h"
)
source_group("Source/Operation/IO" FILES ${OperationIO_Files})

set(MagickaVox_Chunk_Files
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/Chunk.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkFactory.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkLAYR.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkLAYR.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkMAIN.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkMAIN.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkMATL.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkMATL.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunknGRP.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunknGRP.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunknSHP.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunknSHP.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunknTRN.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunknTRN.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkPACK.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkPACK.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkRGBA.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkRGBA.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkrOBJ.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkrOBJ.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkSIZE.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkSIZE.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkXYZI.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkXYZI.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkRCAM.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkRCAM.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkNOTE.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkNOTE.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/Chunk.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/UnkownChunk.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/UnkownChunk.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/Chunk/ChunkFactory.cpp"
)
source_group("Source/Voxel/MagicaVox/Chunk/" FILES ${MagickaVox_Chunk_Files})

set(MagickaVox_IO
    "Source/CADaverLibrary/Voxel/MagicaVox/IO/Reader.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/IO/Reader.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/IO/typeDICT.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/IO/typeDICT.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/IO/typeROTATION.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/IO/typeROTATION.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/IO/Writer.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/IO/Writer.cpp"
)
source_group("Source/Voxel/MagicaVox/IO" FILES ${MagickaVox_IO})

set(MagickaVox_Files
    "Source/CADaverLibrary/Voxel/MagicaVox/VoxFile.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/VoxFile.cpp"
    "Source/CADaverLibrary/Voxel/MagicaVox/VoxFileRaw.h"
    "Source/CADaverLibrary/Voxel/MagicaVox/VoxFileRaw.cpp"
)
source_group("Source/Voxel/MagicaVox" FILES ${MagickaVox_Files})

set(Triangle_Files
    "Source/CADaverLibrary/Triangle/mesh2cad_dumb.h"
    "Source/CADaverLibrary/Triangle/mesh2cad_dumb.cpp"
    "Source/CADaverLibrary/Triangle/mesh2cad_cluster.h"
    "Source/CADaverLibrary/Triangle/mesh2cad_cluster.cpp"
    "Source/CADaverLibrary/Triangle/Clustering.h"
    "Source/CADaverLibrary/Triangle/Clustering.cpp"
    "Source/CADaverLibrary/Triangle/cocoon.h"
    "Source/CADaverLibrary/Triangle/cocoon.cpp"
)
source_group("Source/Triangle" FILES ${Triangle_Files})

set(ALL_FILES
    ${Source_Files}
    ${Util_Files}
    ${Operation_Files}
    ${OperationIO_Files}
    ${CADShapes_Files}
    ${MagickaVox_Files}
    ${MagickaVox_Chunk_Files}
    ${MagickaVox_IO}
    ${Triangle_Files}
)

############################################################################
############################################################################
############################################################################

add_executable(CadaverLibraryTest ${ALL_FILES})
add_library(CadaverLibrary ${ALL_FILES})
set_property(TARGET CadaverLibraryTest PROPERTY CXX_STANDARD 20)
set_property(TARGET CadaverLibrary PROPERTY CXX_STANDARD 20)

target_link_libraries(CadaverLibraryTest PUBLIC 
    CADacomb::CADacomb
    glm::glm
    nlohmann_json::nlohmann_json
    nfd::nfd
)
target_link_libraries(CadaverLibrary PUBLIC 
    CADacomb::CADacomb
    glm::glm
    nlohmann_json::nlohmann_json
    nfd::nfd
)

target_include_directories(CadaverLibrary PUBLIC 
    ./Source/
    SYSTEM ${OpenCASCADE_INCLUDE_DIR}
)
target_include_directories(CadaverLibrary PRIVATE 
    ./Source/Library/
)
target_include_directories(CadaverLibraryTest PUBLIC 
    ./Source/
)
target_include_directories(CadaverLibraryTest PRIVATE 
    ./Source/Library/
)

foreach(LIB ${OpenCASCADE_LIBRARIES})
    if(TARGET ${LIB})
        target_link_libraries(CadaverLibrary PUBLIC ${LIB})
        target_link_libraries(CadaverLibraryTest PRIVATE ${LIB})
    else()
        target_link_libraries(CadaverLibrary PUBLIC ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
        target_link_libraries(CadaverLibrary PUBLIC debug ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
        target_link_libraries(CadaverLibrary PUBLIC optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
        target_link_libraries(CadaverLibraryTest PRIVATE ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
        target_link_libraries(CadaverLibraryTest PRIVATE debug ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
        target_link_libraries(CadaverLibraryTest PRIVATE optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
    endif()
endforeach()

set_property(TARGET CadaverLibraryTest PROPERTY COMPILE_DEFINITIONS ISTESTPROJECT=1)

add_custom_command(TARGET CadaverLibraryTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<IF:$<CONFIG:Debug>,${OpenCASCADE_INSTALL_PREFIX}/debug/bin,${OpenCASCADE_INSTALL_PREFIX}/bin>"
        $<TARGET_FILE_DIR:CadaverLibraryTest>
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug/bin,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin>"
        $<TARGET_FILE_DIR:CadaverLibraryTest>
)

set_property(TARGET CadaverLibraryTest PROPERTY VS_DEBUGGER_ENVIRONMENT 
    "PATH=$<IF:$<CONFIG:Debug>,${OpenCASCADE_INSTALL_PREFIX}/debug/bin,${OpenCASCADE_INSTALL_PREFIX}/bin>;$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug/bin,${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin>;%PATH%"
)

