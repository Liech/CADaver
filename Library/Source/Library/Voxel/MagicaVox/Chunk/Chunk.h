#pragma once

#include <memory>
#include <vector>
#include <string>
#include <fstream>

namespace MagicaVoxImporter {
  class Reader;
  class Chunk {
  public:
    virtual std::string    getID() const = 0;
    virtual void           read(Reader own, Reader child);

    size_t                 numberOfChilds() const;
    const Chunk&           getChild(size_t i) const;

    void                   addChild(std::unique_ptr<Chunk> newchild);

    virtual void           print(int indentation = 0);

    template<typename T> bool childIsType(size_t i) {
      if (i >= _childs.size())
        return false;
      return dynamic_cast<T*>(_childs[i].get()) != nullptr;
    }

    virtual void write(std::vector<unsigned char>&) const;

  private:
    std::vector<std::unique_ptr<Chunk>> _childs;
  };
}