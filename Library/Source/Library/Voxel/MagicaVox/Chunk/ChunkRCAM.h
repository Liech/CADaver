#pragma once

#include <string>
#include <map>

#include "Chunk.h"

//enum class CameraMode {
//  Perspective,// pers
//  Free,       // free
//  Pano,       // pano
//  Iso,        // iso
//  Orthogonal  // orth
//};

namespace MagicaVoxImporter {
  //Render Camera Chunk
  class ChunkRCAM : public  Chunk {
  public:
    std::string getID() const override { return "RCAM"; }
    void        read(Reader own, Reader child) override;

    int                                cameraID;
    std::map<std::string, std::string> attributes;
  };
}