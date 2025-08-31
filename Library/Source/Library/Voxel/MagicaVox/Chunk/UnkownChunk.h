#pragma once

#include "Chunk.h"

namespace MagicaVoxImporter
{
    class UnkownChunk : public Chunk
    {
      public:
        std::string getID() const override
        {
            return "????";
        }
        void read(Reader own, Reader child) override;
    };
}