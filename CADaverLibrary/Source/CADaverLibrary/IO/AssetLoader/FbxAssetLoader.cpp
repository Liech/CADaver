#include "IO/AssetLoader/FbxAssetLoader.h"

#include "CADacomb/IO/FbxReader.h"
#include "CADacomb/IO/FbxSceneData.h"
#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/TextureComponent.h"
#include "Database/Component/UVComponent.h"
#include "Utility/IOValueGrabber.h"

#include <memory>
#include <vector>

FbxAssetLoader::FbxAssetLoader() {}

FbxAssetLoader::~FbxAssetLoader() {}

std::shared_ptr<Asset> FbxAssetLoader::load(const std::string& name, const Polyglot::Node& source) const
{
    CADacomb::ProgressBar bar(3, "Loading FBX asset");

    std::unique_ptr<CADacomb::IO::FbxSceneData> sceneData;
    if (IOValueGrabber::isFilePath(source))
    {
        std::string filePath = IOValueGrabber::grabFilePath(source);
        bar.addProgress(1);
        if (!filePath.empty() && !bar.isCanceled())
        {
            sceneData = CADacomb::IO::FbxReader::loadSceneFromFile(filePath);
        }
    }
    else
    {
        std::vector<uint8_t> bytes = IOValueGrabber::grabBytes(source);
        bar.addProgress(1);
        if (!bytes.empty() && !bar.isCanceled())
        {
            sceneData = CADacomb::IO::FbxReader::loadSceneFromMemory(bytes);
        }
    }

    bar.addProgress(1);

    if (!sceneData || !sceneData->hasTriangulation() || bar.isCanceled())
    {
        return nullptr;
    }

    auto asset = std::make_shared<TriangulationAsset>();
    asset->setName(name);
    asset->setTriangulation(sceneData->takeTriangulation());

    if (sceneData->hasUVs())
    {
        auto uvComponent = std::make_shared<UVComponent>();
        uvComponent->setName("UV");
        uvComponent->setUVs(*sceneData->getUVs());
        asset->addComponent(uvComponent);
    }

    if (sceneData->hasDiffuseTexture())
    {
        auto texComponent = std::make_shared<TextureComponent>();
        texComponent->setName("Texture");
        texComponent->setTexture(std::shared_ptr<CADacomb::Texture::Texture>(sceneData->takeDiffuseTexture()));
        asset->addComponent(texComponent);
    }

    bar.addProgress(1);
    return asset;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

TEST_CASE("FbxAssetLoader/LoadsAsciiFbxIntoTriangulationAsset")
{
    std::string asciiFbx = "; FBX 7.4.0 project file\n"
                           "FBXHeaderExtension:  {\n"
                           "    FBXHeaderVersion: 1003\n"
                           "    FBXVersion: 7400\n"
                           "}\n"
                           "Objects:  {\n"
                           "    Geometry: 100, \"Geometry::Quad\", \"Mesh\" {\n"
                           "        Vertices: *12 {\n"
                           "            a: 0,0,0, 100,0,0, 100,100,0, 0,100,0\n"
                           "        }\n"
                           "        PolygonVertexIndex: *4 {\n"
                           "            a: 0,1,2,-4\n"
                           "        }\n"
                           "    }\n"
                           "    Model: 200, \"Model::Quad\", \"Mesh\" {\n"
                           "        Version: 232\n"
                           "    }\n"
                           "}\n"
                           "Connections:  {\n"
                           "    C: \"OO\",100,200\n"
                           "    C: \"OO\",200,0\n"
                           "}\n";

    std::vector<uint8_t> bytes(asciiFbx.begin(), asciiFbx.end());
    FbxAssetLoader       loader;
    auto                 loaded = std::dynamic_pointer_cast<TriangulationAsset>(loader.load("QuadFbx", Polyglot::Node(bytes)));
    REQUIRE(loaded != nullptr);
    CHECK(loaded->getName() == "QuadFbx");
    CHECK(loaded->getType() == "Triangulation");
    REQUIRE(loaded->getTriangulation().vertices.size() == 4);
    REQUIRE(loaded->getTriangulation().indices.size() == 6);
    CHECK(loaded->getTriangulation().vertices[2].x == Catch::Approx(1.0));
    CHECK(loaded->getTriangulation().vertices[2].y == Catch::Approx(1.0));
}

TEST_CASE("FbxReader/ExtractsUVsAndExternalPngTexture")
{
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "cadacomb_fbx_uv_tex_test";
    std::filesystem::create_directories(tempDir);

    std::filesystem::path      pngPath = tempDir / "albedo.png";
    CADacomb::Texture::Texture expectedTex(2, 2, glm::u8vec4(40, 80, 120, 255));
    expectedTex.setPixel(1, 0, glm::u8vec4(250, 10, 20, 255));
    expectedTex.saveToPngFile(pngPath.string());

    std::string asciiFbx = "; FBX 7.4.0 project file\n"
                           "FBXHeaderExtension:  {\n"
                           "    FBXHeaderVersion: 1003\n"
                           "    FBXVersion: 7400\n"
                           "}\n"
                           "Objects:  {\n"
                           "    Geometry: 100, \"Geometry::Tri\", \"Mesh\" {\n"
                           "        Vertices: *9 {\n"
                           "            a: 0,0,0, 100,0,0, 0,100,0\n"
                           "        }\n"
                           "        PolygonVertexIndex: *3 {\n"
                           "            a: 0,1,-3\n"
                           "        }\n"
                           "        LayerElementUV: 0 {\n"
                           "            Version: 101\n"
                           "            Name: \"UVMap\"\n"
                           "            MappingInformationType: \"ByPolygonVertex\"\n"
                           "            ReferenceInformationType: \"Direct\"\n"
                           "            UV: *6 {\n"
                           "                a: 0.0,0.0, 1.0,0.0, 0.5,1.0\n"
                           "            }\n"
                           "        }\n"
                           "        Layer: 0 {\n"
                           "            Version: 100\n"
                           "            LayerElement:  {\n"
                           "                Type: \"LayerElementUV\"\n"
                           "                TypedIndex: 0\n"
                           "            }\n"
                           "        }\n"
                           "    }\n"
                           "    Model: 200, \"Model::Tri\", \"Mesh\" {\n"
                           "        Version: 232\n"
                           "    }\n"
                           "    Material: 300, \"Material::Mat\", \"\" {\n"
                           "        Version: 102\n"
                           "        ShadingModel: \"phong\"\n"
                           "    }\n"
                           "    Texture: 400, \"Texture::Tex\", \"\" {\n"
                           "        Type: \"TextureVideoClip\"\n"
                           "        Version: 202\n"
                           "        RelativeFilename: \"albedo.png\"\n"
                           "    }\n"
                           "}\n"
                           "Connections:  {\n"
                           "    C: \"OO\",100,200\n"
                           "    C: \"OO\",200,0\n"
                           "    C: \"OO\",300,200\n"
                           "    C: \"OP\",400,300, \"DiffuseColor\"\n"
                           "}\n";

    std::filesystem::path fbxPath = tempDir / "textured_tri.fbx";
    {
        std::ofstream out(fbxPath, std::ios::binary);
        out.write(asciiFbx.data(), static_cast<std::streamsize>(asciiFbx.size()));
    }

    auto           sceneData = CADacomb::IO::FbxReader::loadSceneFromFile(fbxPath.string());
    FbxAssetLoader loader;
    auto           loadedAsset = std::dynamic_pointer_cast<TriangulationAsset>(loader.load("TexturedTri", Polyglot::Node(fbxPath.string())));
    std::filesystem::remove_all(tempDir);

    REQUIRE(sceneData != nullptr);
    REQUIRE(sceneData->hasTriangulation());
    REQUIRE(sceneData->hasUVs());
    REQUIRE(sceneData->getUVs()->getCornerCount() == 3);
    CHECK(sceneData->getUVs()->getUV(0).x == Catch::Approx(0.0));
    CHECK(sceneData->getUVs()->getUV(0).y == Catch::Approx(1.0));
    CHECK(sceneData->getUVs()->getUV(2).x == Catch::Approx(0.5));
    CHECK(sceneData->getUVs()->getUV(2).y == Catch::Approx(0.0));

    REQUIRE(sceneData->hasDiffuseTexture());
    CHECK(sceneData->getDiffuseTexture()->getWidth() == 2);
    CHECK(sceneData->getDiffuseTexture()->getHeight() == 2);
    CHECK(sceneData->getDiffuseTexture()->getPixel(1, 0) == glm::u8vec4(250, 10, 20, 255));

    REQUIRE(loadedAsset != nullptr);
    auto attachedUVs = loadedAsset->getAttachments<UVComponent>();
    REQUIRE(attachedUVs.size() == 1);
    CHECK(attachedUVs[0]->getUVs().getCornerCount() == 3);

    auto attachedTextures = loadedAsset->getAttachments<TextureComponent>();
    REQUIRE(attachedTextures.size() == 1);
    CHECK(attachedTextures[0]->getTexture().getPixel(1, 0) == glm::u8vec4(250, 10, 20, 255));
}
#endif
