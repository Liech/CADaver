cmake_minimum_required(VERSION 3.19)
project(LibraryGD)

set(Source_Files
    "Source/register_types.h"
    "Source/register_types.cpp"
    "Source/VoxelShape.h"
    "Source/VoxelShape.cpp"
)
source_group("Source" FILES ${Source_Files})

set(CADShape_Files
    "Source/CADShape/CADShape.cpp"
    "Source/CADShape/CADShape.h"
    "Source/CADShape/CADCompound.cpp"
    "Source/CADShape/CADCompound.h"
    "Source/CADShape/CADCompSolid.cpp"
    "Source/CADShape/CADCompSolid.h"
    "Source/CADShape/CADEdge.cpp"
    "Source/CADShape/CADEdge.h"
    "Source/CADShape/CADFace.cpp"
    "Source/CADShape/CADFace.h"
    "Source/CADShape/CADShell.cpp"
    "Source/CADShape/CADShell.h"
    "Source/CADShape/CADSolid.cpp"
    "Source/CADShape/CADSolid.h"
    "Source/CADShape/CADVertex.cpp"
    "Source/CADShape/CADVertex.h"
    "Source/CADShape/CADWire.cpp"
    "Source/CADShape/CADWire.h"
    "Source/CADShape/CADShapeFactory.cpp"
    "Source/CADShape/CADShapeFactory.h"
)
source_group("Source/CADShape" FILES ${CADShape_Files})

set(Util_Files
    "Source/Util/SaveFileDialog.h"
    "Source/Util/SaveFileDialog.cpp"
    "Source/Util/LoadFileDialog.h"
    "Source/Util/LoadFileDialog.cpp"
    "Source/Util/CADOperation.h"
    "Source/Util/CADOperation.cpp"
)
source_group("Source/Util" FILES ${Util_Files})

set(ALL_FILES
    ${Util_Files}
    ${Source_Files}
    ${CADShape_Files}
)

add_library(LibraryGD SHARED ${ALL_FILES})
set_property(TARGET LibraryGD PROPERTY DBG_POSTFIX d)
set_property(TARGET LibraryGD PROPERTY CXX_STANDARD 20)


target_include_directories(LibraryGD PRIVATE 
"${CMAKE_CURRENT_SOURCE_DIR}/src"
./../Library/Source
./Source/
)

target_link_libraries(LibraryGD PUBLIC 
godot::cpp 
Library)

add_custom_command(TARGET LibraryGD POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_directory 
               $<TARGET_FILE_DIR:LibraryGD>              
                ${CMAKE_SOURCE_DIR}/godotProject/bin)