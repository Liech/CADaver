#include "ChunknTRN.h"
#include "Util/MagicaVox/IO/Reader.h"
#include <stdexcept>

namespace MagicaVoxImporter {
  void ChunknTRN::read(Reader own, Reader child) {
    Chunk::read(own, child);
    ID = own.readInt();
    auto attributes = own.readDict();
    if (attributes.count("_name")) Name = attributes["_name"]; else Name = "Unnamed";
    if (attributes.count("_hidden")) Hidden = attributes["_hidden"] == "1"; else Hidden = false;
    ChildID = own.readInt();
    int dummy = own.readInt();
    //if (dummy != -1)
    //  throw std::runtime_error("Reserved int must be -1");
    LayerID = own.readInt();
    int numFrames = own.readInt(); //must be 1
    if (numFrames != 1)
      throw std::runtime_error("Number of Frames must be 1");
    auto frameAttributes = own.readDict();
    if (frameAttributes.count("_t")) Translation = frameAttributes["_t"];
    if (frameAttributes.count("_r")) Rotation = frameAttributes["_r"];
  }
}