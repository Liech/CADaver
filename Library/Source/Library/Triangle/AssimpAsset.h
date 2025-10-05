#pragma once

#include <memory>
#include <string>
#include <vector>

struct aiNode;
struct aiScene;

namespace Library
{
    class Triangulation;

    class AssimpAsset
    {
      public:
        AssimpAsset();
        virtual ~AssimpAsset();

        void                           save(const std::string& filename);
        std::unique_ptr<Triangulation> toTriangulation() const;

        static std::unique_ptr<AssimpAsset> load(const std::string& filename);
        static std::vector<std::string>     getSupportedFormats();

      private:
        static std::string getAssimpFormatIdFromFilePath(const std::string& filePath);

        void toTriangulation_recursive(aiNode* node, const aiScene* scene, Triangulation& data, int& baseVertexIndex) const;
        

        class pimpl; // to hide assimp
        std::unique_ptr<pimpl> p = nullptr;
    };
}