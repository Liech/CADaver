#include "AssimpAsset.h"

#include "Triangulation.h"

#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <filesystem>
#include <iostream>

namespace Library
{
    class AssimpAsset::pimpl
    {
      public:
        Assimp::Importer importer;
        aiScene*         scene = nullptr;
    };

    AssimpAsset::AssimpAsset()
    {
        p = std::make_unique<AssimpAsset::pimpl>();
    }

    AssimpAsset ::~AssimpAsset()
    {
        if (p->scene)
            aiReleaseImport(p->scene);
    }

    std::unique_ptr<AssimpAsset> AssimpAsset::load(const std::string& filename)
    {
        std::unique_ptr<AssimpAsset> result = std::make_unique<AssimpAsset>();
        auto                         scene  = result->p->importer.ReadFile(filename,
                                                  aiProcess_Triangulate |           // Convert all primitives to triangles
                                                    aiProcess_GenSmoothNormals |    // Generate smooth normals if they don't exist
                                                    aiProcess_FlipUVs |             // Flip texture coordinates along the Y-axis
                                                    aiProcess_JoinIdenticalVertices // Join identical vertices in the mesh
        );

        result->p->scene = result->p->importer.GetOrphanedScene();

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "Error loading model: " << result->p->importer.GetErrorString() << std::endl;
            return nullptr;
        }

        return std::move(result);
    }

    std::string AssimpAsset::getAssimpFormatIdFromFilePath(const std::string& filePath)
    {
        Assimp::Exporter exporter;

        std::filesystem::path pathObj(filePath);
        std::string           ext = pathObj.extension().string();

        if (!ext.empty() && ext[0] == '.')
        {
            ext = ext.substr(1);
        }
        if (ext.empty())
        {
            return "";
        }
        size_t formatCount = exporter.GetExportFormatCount();
        for (size_t i = 0; i < formatCount; ++i)
        {
            const aiExportFormatDesc* desc      = exporter.GetExportFormatDescription(i);
            std::string               assimpExt = desc->fileExtension;
            if (std::tolower(ext[0]) == std::tolower(assimpExt[0]) && // Quick check for first letter
                std::equal(ext.begin(), ext.end(), assimpExt.begin(), assimpExt.end(), [](char a, char b) { return std::tolower(a) == std::tolower(b); }))
            {
                return desc->id;
            }
        }
        return "";
    }

    void AssimpAsset::save(const std::string& filename)
    {
        auto             formatId = getAssimpFormatIdFromFilePath(filename);
        Assimp::Exporter exporter;
        aiReturn         result = exporter.Export(p->scene,         // The scene to export
                                          formatId.c_str(), // ID of the export format (e.g., "obj", "fbx", "dae", "assbin")
                                          filename.c_str(), // Output file path
                                          0                 // Optional: Post-processing flags, can be 0 or other flags
        );
        if (result != AI_SUCCESS)
        {
            std::cerr << "Error exporting scene: " << exporter.GetErrorString() << std::endl;
        }
    }

    std::vector<std::string> AssimpAsset::getSupportedFormats()
    {
        return { "fbx",       "dae", "gltf", "glb",  "blend", "3ds", "ase", "obj", "ifc",     "xgl", "zgl", "ply", "dxf", "lwo", "lws", "lxo",     "stl",
                 "x",         "ac",  "ms3d", "cob",  "scn",   "bvh", "csm", "xml", "irrmesh", "irr", "mdl", "md2", "md3", "pk3", "mdc", "md5mesh", "md5anim",
                 "md5camera", "smd", "vta",  "ogex", "3d",    "b3d", "q3d", "q3s", "nff",     "off", "raw", "ter", "hmp", "ndo", "3mf", "q3o",     "q3" };
    }

    std::unique_ptr<Triangulation> AssimpAsset::toTriangulation() const
    {
        std::unique_ptr<Triangulation> result = std::make_unique<Triangulation>();

        if (!p->scene || p->scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !p->scene->mRootNode)
        {
            return nullptr;
        }

        int baseVertexIndex = 0;
        toTriangulation_recursive(p->scene->mRootNode, p->scene, *result, baseVertexIndex);
        return std::move(result);
    }

    void AssimpAsset::toTriangulation_recursive(aiNode* node, const aiScene* scene, Triangulation& data, int& baseVertexIndex) const
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh                  = scene->mMeshes[node->mMeshes[i]];
            int     currentMeshVertexBase = data.vertices.size();

            for (unsigned int j = 0; j < mesh->mNumVertices; j++)
            {
                aiVector3D pos = mesh->mVertices[j];
                data.vertices.push_back(glm::dvec3(pos.x, pos.y, pos.z));
            }

            for (unsigned int j = 0; j < mesh->mNumFaces; j++)
            {
                aiFace face = mesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; k++)
                {
                    int globalIndex = currentMeshVertexBase + face.mIndices[k];
                    data.indices.push_back(globalIndex);
                }
            }
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            toTriangulation_recursive(node->mChildren[i], scene, data, baseVertexIndex);
        }
    }
}