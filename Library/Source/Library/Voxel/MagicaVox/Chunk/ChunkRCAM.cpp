#include "ChunkRCAM.h"
#include "Voxel/MagicaVox/IO/Reader.h"

#include <iostream>
#include <string>

namespace MagicaVoxImporter
{
    void ChunkRCAM::read(Reader own, Reader child)
    {
        Chunk::read(own, child);

        cameraID = own.readInt();

        attributes = own.readDict();

        // std::string modeStr = attributes["_mode"];
        // if (modeStr == "pers")
        //   mode = CameraMode::Perspective;
        // else if (modeStr == "free")
        //   mode = CameraMode::Perspective;
        // else if (modeStr == "pano")
        //   mode = CameraMode::Pano;
        // else if (modeStr == "iso")
        //   mode = CameraMode::Iso;
        // else if (modeStr == "orth")
        //   mode = CameraMode::Orthogonal
        // else
        //   throw std::runtime_error("Unkown Camera Mode");
    }
}