#pragma once

#include <memory>
#include <string>

namespace Library
{
  class AssimpAsset
  {
    public:
      AssimpAsset();
      virtual ~AssimpAsset();

      void save(const std::string& filename);

      static std::unique_ptr<AssimpAsset> load(const std::string& filename);
    private:
      static std::string getAssimpFormatIdFromFilePath(const std::string& filePath);


      class pimpl; // to hide assimp
      std::unique_ptr<pimpl> p = nullptr;
  };
}