#include <iostream>

#include "Library/CAD/CADShape.h"
#include "Library/CAD/CADShapeFactory.h"
#include "Library/Operation/TriangulateOperation.h"
#include "Library/Util/LoadFileDialog.h"
#include "Library/Util/SaveFileDialog.h"
#include "Library/Triangle/Triangulation.h"
#include "Library/Voxel/MagicaVox/VoxFile.h"
#include "Library/Voxel/BinaryVolume.h"
#include "Library/Operation/LoadVoxelOperation.h"

int main()
{
    Library::LoadFileDialog dlg;
    dlg.addFilter("vox files", { "VOX" });
    dlg.execute();
    if (!dlg.isCancled())
    {
        std::shared_ptr<Library::BinaryVolume> result = Library::LoadVoxelOperation::loadMagicaVox(dlg.getResultPath());
        auto triangulation = Library::TriangulateOperation::triangulateBlocky(*result);
        triangulation->saveAsSTL(dlg.getResultPath() + ".stl");
    }
    std::cout << dlg.getResultPath();
}