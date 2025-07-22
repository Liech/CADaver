#include "VoxFile.h"

#include <memory>
#include <cassert>
#include <fstream>

#include "Chunk/ChunkMAIN.h"
#include "Chunk/ChunkPACK.h"
#include "Chunk/ChunkSIZE.h"
#include "Chunk/ChunkXYZI.h"
#include "Chunk/ChunkRGBA.h"
#include "Chunk/ChunkMATL.h"

#include "VoxFileRaw.h"


namespace MagicaVoxImporter {
  VoxFile::VoxFile(const std::string& filename) {
    std::unique_ptr<Chunk> parsedFile = VoxFileRaw::read(filename);
    //parsedFile->print(1);
    size_t currentChunk = 0;
    int numberOfModels = 1;
    if (parsedFile->childIsType<ChunkPACK>(currentChunk)) {
      const ChunkPACK& pack = (const ChunkPACK&)(parsedFile->getChild(currentChunk));
      numberOfModels = pack.numModels;
      currentChunk++;
    }

    for (int i = 0; i < numberOfModels; i++) {
      const ChunkSIZE& size = (const ChunkSIZE&)parsedFile->getChild(currentChunk);
      currentChunk++;
      ChunkXYZI& xyzi = (ChunkXYZI&)parsedFile->getChild(currentChunk);
      currentChunk++;
      std::vector<unsigned char> content;
      size_t dataSize = (size_t)size.sizeX * (size_t)size.sizeY * (size_t)size.sizeZ;
      content.resize(dataSize);
      for (int j = 0; j < xyzi.numVoxels; j++) {
        size_t address = xyzi.content[j].Z + xyzi.content[j].Y * size.sizeZ + xyzi.content[j].X * size.sizeZ * size.sizeY;
        content[address] = xyzi.content[j].value;
      }
      std::array<size_t, 3> dimensions = { size.sizeX,size.sizeY,size.sizeZ };
      Models.push_back(std::make_pair(content, dimensions));
    }


    if (!parsedFile->childIsType<ChunkRGBA>(currentChunk)) {
      auto rgba = ChunkRGBA::getDefault();
      for (int i = 0; i < 256; i++)
        Colors[i] = std::array<unsigned char, 4>{rgba->palette[i].r, rgba->palette[i].g, rgba->palette[i].b, rgba->palette[i].a};
    }
    else {
      const ChunkRGBA& rgba = (const ChunkRGBA&)parsedFile->getChild(currentChunk);
      for (int i = 0; i < 256; i++)
        Colors[i] = std::array<unsigned char, 4>{rgba.palette[i].r, rgba.palette[i].g, rgba.palette[i].b, rgba.palette[i].a};
      currentChunk++;
    }


    Materials.resize(256);
    while (currentChunk < parsedFile->numberOfChilds()) {
      if (parsedFile->childIsType<ChunkMATL>(currentChunk)) {
        const ChunkMATL& mat = (const ChunkMATL&)parsedFile->getChild(currentChunk);
        Materials[mat.MaterialID-1] = mat.Properties;
      }
      currentChunk++;
    }

  }

  VoxFile::~VoxFile() {

  }

  std::pair<std::unique_ptr<bool[]>, std::array<size_t, 3>> VoxFile::readBinary(const std::string& filename) {
    VoxFile v(filename);
    assert(v.Models.size() == 1);
    std::array<size_t, 3> dimensions = v.Models[0].second;  
    size_t dataSize = dimensions[0] * dimensions[1] * dimensions[2];
    std::unique_ptr<bool[]> content = std::make_unique<bool[]>(dataSize);
    for (size_t i = 0; i < dataSize; i++)
      content[i] = v.Models[0].first[i] != 0;
    return std::make_pair(std::move(content),dimensions);
  }

  void VoxFile::writeBinary(bool* data, const std::array<size_t, 3>& size, const std::string& filename) {
    ChunkMAIN mainChunk;
    std::unique_ptr<ChunkSIZE> sizeChunk = std::make_unique<ChunkSIZE>();
    sizeChunk->sizeX = size[0];
    sizeChunk->sizeY = size[1];
    sizeChunk->sizeZ = size[2];
    
    std::unique_ptr<ChunkXYZI> contentChunk = std::make_unique<ChunkXYZI>();
    size_t dataSize = size[0] * size[1] * size[2];
    contentChunk->numVoxels = dataSize;
    contentChunk->content.resize(dataSize);
    for (size_t i = 0; i < dataSize; i++) {
      auto& vox = contentChunk->content[i];
      vox.X = i / (size[1] * size[2]);
      vox.Y = (i / size[2]) % size[1];
      vox.Z = i % size[2];
      vox.value = data[i]?'U':0;
    }

    mainChunk.addChild(std::move(sizeChunk));
    mainChunk.addChild(std::move(contentChunk));

    VoxFileRaw::write(mainChunk, filename);
  }
}