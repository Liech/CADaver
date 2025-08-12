#include "ChunkMAIN.h"

#include "Voxel/MagicaVox/IO/Writer.h"

namespace MagicaVoxImporter {

  void ChunkMAIN::write(std::vector<unsigned char>& file) const {
    std::vector<unsigned char> childContent;

    for (size_t i = 0; i < numberOfChilds(); i++) {
      const auto& child = getChild(i);
      child.write(childContent);
    }

    Writer::write(file,getID());
    Writer::write(file, 0);//own content size
    Writer::write(file, childContent.size()); //child size
    file.insert(file.end(), childContent.begin(), childContent.end());
  }
}