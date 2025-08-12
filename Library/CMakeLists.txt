cmake_minimum_required(VERSION 2.8...3.20)

find_package(Catch2 REQUIRED)

project(Library)
#project(LibraryTest)

find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(nfd CONFIG REQUIRED) 
find_package(OpenCASCADE)
include_directories(SYSTEM ${OpenCASCADE_INCLUDE_DIR})


set(Source_Files
    "Source/Library/testMain.cpp"
)
source_group("Source" FILES ${Source_Files})

set(CADShapes_Files
    "Source/Library/CAD/CADShape.cpp"
    "Source/Library/CAD/CADShape.h"
    "Source/Library/CAD/CADCompound.cpp"
    "Source/Library/CAD/CADCompound.h"
    "Source/Library/CAD/CADCompSolid.cpp"
    "Source/Library/CAD/CADCompSolid.h"
    "Source/Library/CAD/CADEdge.cpp"
    "Source/Library/CAD/CADEdge.h"
    "Source/Library/CAD/CADFace.cpp"
    "Source/Library/CAD/CADFace.h"
    "Source/Library/CAD/CADShell.cpp"
    "Source/Library/CAD/CADShell.h"
    "Source/Library/CAD/CADSolid.cpp"
    "Source/Library/CAD/CADSolid.h"
    "Source/Library/CAD/CADVertex.cpp"
    "Source/Library/CAD/CADVertex.h"
    "Source/Library/CAD/CADWire.cpp"
    "Source/Library/CAD/CADWire.h"
    "Source/Library/CAD/TopAbs2String.cpp"
    "Source/Library/CAD/TopAbs2String.h"
    "Source/Library/CAD/CADShapeFactory.cpp"
    "Source/Library/CAD/CADShapeFactory.h"
)
source_group("Source/CAD" FILES ${CADShapes_Files})

set(Util_Files
    "Source/Library/Util/SaveFileDialog.cpp"
    "Source/Library/Util/SaveFileDialog.h"
    "Source/Library/Util/LoadFileDialog.cpp"
    "Source/Library/Util/LoadFileDialog.h"
)
source_group("Source/Util" FILES ${Util_Files})

set(Operation_Files
    "Source/Library/Operation/ExtrudeOperation.cpp"
    "Source/Library/Operation/ExtrudeOperation.h"
    "Source/Library/Operation/TriangulateOperation.cpp"
    "Source/Library/Operation/TriangulateOperation.h"
    "Source/Library/Operation/LoadCADOperation.cpp"
    "Source/Library/Operation/LoadCADOperation.h"
    "Source/Library/Operation/SaveCADOperation.cpp"
    "Source/Library/Operation/SaveCADOperation.h"
    "Source/Library/Operation/LoadVoxelOperation.cpp"
    "Source/Library/Operation/LoadVoxelOperation.h"
    "Source/Library/Operation/SaveVoxelOperation.cpp"
    "Source/Library/Operation/SaveVoxelOperation.h"
)
source_group("Source/Operation" FILES ${Operation_Files})

set(MagickaVox_Chunk_Files
    "Source/Library/Voxel/MagicaVox/Chunk/Chunk.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkFactory.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkLAYR.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkLAYR.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkMAIN.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkMAIN.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkMATL.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkMATL.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunknGRP.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunknGRP.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunknSHP.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunknSHP.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunknTRN.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunknTRN.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkPACK.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkPACK.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkRGBA.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkRGBA.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkrOBJ.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkrOBJ.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkSIZE.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkSIZE.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkXYZI.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkXYZI.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkRCAM.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkRCAM.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkNOTE.h"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkNOTE.cpp"
	  "Source/Library/Voxel/MagicaVox/Chunk/Chunk.cpp"
    "Source/Library/Voxel/MagicaVox/Chunk/ChunkFactory.cpp"
)
source_group("Source/Voxel/MagicaVox/Chunk/" FILES ${MagickaVox_Chunk_Files})

set(MagickaVox_IO
    "Source/Library/Voxel/MagicaVox/IO/Reader.h"
    "Source/Library/Voxel/MagicaVox/IO/Reader.cpp"
    "Source/Library/Voxel/MagicaVox/IO/typeDICT.h"
    "Source/Library/Voxel/MagicaVox/IO/typeDICT.cpp"
    "Source/Library/Voxel/MagicaVox/IO/typeROTATION.h"
    "Source/Library/Voxel/MagicaVox/IO/typeROTATION.cpp"
    "Source/Library/Voxel/MagicaVox/IO/Writer.h"
    "Source/Library/Voxel/MagicaVox/IO/Writer.cpp"
)
source_group("Source/Voxel/MagicaVox/IO" FILES ${MagickaVox_IO})

set(MagickaVox_Files
    "Source/Library/Voxel/MagicaVox/VoxFile.h"
    "Source/Library/Voxel/MagicaVox/VoxFile.cpp"
    "Source/Library/Voxel/MagicaVox/VoxFileRaw.h"
    "Source/Library/Voxel/MagicaVox/VoxFileRaw.cpp"
)
source_group("Source/Voxel/MagicaVox" FILES ${MagickaVox_Files})

set(Voxel_Files
    "Source/Library/Voxel/BinaryVolume.h"
    "Source/Library/Voxel/BinaryVolume.cpp"
    "Source/Library/Voxel/MarchingCubes.h"
    "Source/Library/Voxel/MarchingCubes.cpp"
)
source_group("Source/Voxel" FILES ${Voxel_Files})

set(Triangle_Files
    "Source/Library/Triangle/Triangulation.h"
    "Source/Library/Triangle/Triangulation.cpp"
    "Source/Library/Triangle/STLWriter.h"
    "Source/Library/Triangle/STLWriter.cpp"
)
source_group("Source/Triangle" FILES ${Triangle_Files})

set(ALL_FILES
    ${Source_Files}
    ${Util_Files}
    ${Operation_Files}
    ${CADShapes_Files}
	  ${MagickaVox_Files}
	  ${MagickaVox_Chunk_Files}
	  ${MagickaVox_IO}
	  ${Voxel_Files}
	  ${Triangle_Files}
)

############################################################################
############################################################################
############################################################################

#add_executable(LibraryTest ${ALL_FILES})
add_library(Library ${ALL_FILES})
#set_property(TARGET LibraryTest PROPERTY CXX_STANDARD 20)
set_property(TARGET Library PROPERTY CXX_STANDARD 20)

#target_link_libraries(LibraryTest PUBLIC 
#Catch2::Catch2
#glm::glm
#nlohmann_json::nlohmann_json
#nfd::nfd
#)
target_link_libraries(Library PUBLIC 
glm::glm
nlohmann_json::nlohmann_json
nfd::nfd
)

target_include_directories(Library PUBLIC 
./Source/
)
target_include_directories(Library PRIVATE 
./Source/Library/
)
#target_include_directories(LibraryTest PUBLIC 
#./Source/
#)
#target_include_directories(LibraryTest PRIVATE 
#./Source/Library/
#)

#https://www.youtube.com/watch?v=SfRFG_Pk9pk
#https://dev.opencascade.org/content/buildcompile-application
foreach(LIB ${OpenCASCADE_LIBRARIES})
target_link_libraries(Library PRIVATE ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
target_link_libraries(Library PRIVATE debug ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
target_link_libraries(Library PRIVATE optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
#target_link_libraries(LibraryTest PRIVATE ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
#target_link_libraries(LibraryTest PRIVATE debug ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
#target_link_libraries(LibraryTest PRIVATE optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

#set_property( TARGET LibraryTest PROPERTY COMPILE_DEFINITIONS ISTESTPROJECT=1 )


#include(CTest)
#include(Catch)
#catch_discover_tests(LibraryTest)