#include <iostream>

#include "Library/CAD/CADShape.h"
#include "Library/CAD/CADShapeFactory.h"
#include "Library/Operation/TriangulateOperation.h"
#include "Library/Util/LoadFileDialog.h"
#include "Library/Util/SaveFileDialog.h"
#include "Library/Triangle/Triangulation.h"
#include "Library/Voxel/MagicaVox/VoxFile.h"


int main()
{
    Library::LoadFileDialog dlg;
    dlg.addFilter("vox files", { "VOX" });
    dlg.execute();
    std::cout << "Cancel: " << dlg.isCancled() << std::endl;

    if (!dlg.isCancled())
    {
        auto shape = MagicaVoxImporter::VoxFile::readBinary(dlg.getResultPath());
    }
    std::cout << dlg.getResultPath();
}