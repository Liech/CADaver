#include <iostream>

#include "Library/Util/LoadFileDialog.h"
#include "Library/Util/SaveFileDialog.h"
#include "Library/Triangle/Triangulation.h"
#include "Library/Operation/IO/LoadVoxelOperation.h"
#include "Library/Operation/IO/SaveVoxelOperation.h"
#include "Library/Voxel/BinaryVolume.h"

int main()
{
      Library::LoadFileDialog dlg;
    dlg.addFilter("stl files", { "stl" });

    dlg.execute();
    if (!dlg.isCancled())
    {
        std::cout << "file: " << dlg.getResultPath() << std::endl;
        auto stl = Library::Triangulation::fromSTLFile(dlg.getResultPath());
        std::shared_ptr<Library::BinaryVolume> resultShape = Library::LoadVoxelOperation::voxelize(*stl, glm::ivec3(100,100,100));
        Library::SaveVoxelOperation::saveMagicaVox(*resultShape, dlg.getResultPath() + ".vox");
    }
    std::cout << dlg.getResultPath();
}