#include "IO/AssetLoader/FbxAssetLoader.h"

#include "CADacomb/IO/FbxReader.h"
#include "CADacomb/IO/FbxSceneData.h"
#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/AnimationComponent.h"
#include "Database/Component/SkeletonComponent.h"
#include "Database/Component/TextureComponent.h"
#include "Database/Component/TriangulationSkinningComponent.h"
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

    if (sceneData->hasSkeleton())
    {
        auto skeletonComponent = std::make_shared<SkeletonComponent>();
        skeletonComponent->setName("Skeleton");
        skeletonComponent->setSkeleton(*sceneData->getSkeleton());
        asset->addComponent(skeletonComponent);
    }

    if (sceneData->hasSkinning())
    {
        auto skinningComponent = std::make_shared<TriangulationSkinningComponent>();
        skinningComponent->setName("TriangulationSkinning");
        skinningComponent->setSkinning(*sceneData->getSkinning());
        asset->addComponent(skinningComponent);
    }

    if (sceneData->hasAnimationClips())
    {
        auto animationComponent = std::make_shared<AnimationComponent>();
        animationComponent->setName("Animation");
        animationComponent->setClips(sceneData->getAnimationClips());
        asset->addComponent(animationComponent);
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

TEST_CASE("FbxReader/ExtractsSkeletonSkinningAndAnimationClips")
{
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
                           "    }\n"
                           "    Model: 200, \"Model::Tri\", \"Mesh\" {\n"
                           "        Version: 232\n"
                           "    }\n"
                           "    NodeAttribute: 301, \"NodeAttribute::RootBone\", \"LimbNode\" {\n"
                           "        TypeFlags: \"Skeleton\"\n"
                           "    }\n"
                           "    Model: 300, \"Model::RootBone\", \"LimbNode\" {\n"
                           "        Version: 232\n"
                           "    }\n"
                           "    NodeAttribute: 311, \"NodeAttribute::ChildBone\", \"LimbNode\" {\n"
                           "        TypeFlags: \"Skeleton\"\n"
                           "    }\n"
                           "    Model: 310, \"Model::ChildBone\", \"LimbNode\" {\n"
                           "        Version: 232\n"
                           "        Properties70:  {\n"
                           "            P: \"Lcl Translation\", \"Lcl Translation\", \"\", \"A\",0,100,0\n"
                           "        }\n"
                           "    }\n"
                           "    Deformer: 400, \"Deformer::Skin\", \"Skin\" {\n"
                           "        Version: 101\n"
                           "    }\n"
                           "    Deformer: 410, \"SubDeformer::ChildCluster\", \"Cluster\" {\n"
                           "        Version: 100\n"
                           "        Indexes: *3 {\n"
                           "            a: 0,1,2\n"
                           "        }\n"
                           "        Weights: *3 {\n"
                           "            a: 1.0,1.0,1.0\n"
                           "        }\n"
                           "        Transform: *16 {\n"
                           "            a: 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,-100,0,1\n"
                           "        }\n"
                           "        TransformLink: *16 {\n"
                           "            a: 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,100,0,1\n"
                           "        }\n"
                           "    }\n"
                           "    AnimationStack: 500, \"AnimStack::Lift\", \"\" {\n"
                           "        Properties70:  {\n"
                           "            P: \"LocalStop\", \"KTime\", \"Time\", \"\",46186158000\n"
                           "        }\n"
                           "    }\n"
                           "    AnimationLayer: 510, \"AnimLayer::BaseLayer\", \"\" {\n"
                           "    }\n"
                           "    AnimationCurveNode: 520, \"AnimCurveNode::T\", \"\" {\n"
                           "        Properties70:  {\n"
                           "            P: \"d|X\", \"Number\", \"\", \"A\",0\n"
                           "            P: \"d|Y\", \"Number\", \"\", \"A\",100\n"
                           "            P: \"d|Z\", \"Number\", \"\", \"A\",0\n"
                           "        }\n"
                           "    }\n"
                           "    AnimationCurve: 530, \"AnimCurve::\", \"\" {\n"
                           "        Default: 100\n"
                           "        KeyVer: 4008\n"
                           "        KeyTime: *2 {\n"
                           "            a: 0,46186158000\n"
                           "        }\n"
                           "        KeyValueFloat: *2 {\n"
                           "            a: 100,300\n"
                           "        }\n"
                           "        KeyAttrFlags: *1 {\n"
                           "            a: 8456\n"
                           "        }\n"
                           "        KeyAttrDataFloat: *4 {\n"
                           "            a: 0,0,0,0\n"
                           "        }\n"
                           "        KeyAttrRefCount: *1 {\n"
                           "            a: 2\n"
                           "        }\n"
                           "    }\n"
                           "}\n"
                           "Connections:  {\n"
                           "    C: \"OO\",100,200\n"
                           "    C: \"OO\",200,0\n"
                           "    C: \"OO\",301,300\n"
                           "    C: \"OO\",300,0\n"
                           "    C: \"OO\",311,310\n"
                           "    C: \"OO\",310,300\n"
                           "    C: \"OO\",400,100\n"
                           "    C: \"OO\",410,400\n"
                           "    C: \"OO\",310,410\n"
                           "    C: \"OO\",510,500\n"
                           "    C: \"OO\",520,510\n"
                           "    C: \"OP\",520,310, \"Lcl Translation\"\n"
                           "    C: \"OP\",530,520, \"d|Y\"\n"
                           "}\n";

    std::vector<uint8_t> bytes(asciiFbx.begin(), asciiFbx.end());
    auto                 sceneData = CADacomb::IO::FbxReader::loadSceneFromMemory(bytes);

    REQUIRE(sceneData != nullptr);
    REQUIRE(sceneData->hasTriangulation());
    REQUIRE(sceneData->hasSkeleton());
    REQUIRE(sceneData->hasSkinning());
    REQUIRE(sceneData->hasAnimationClips());

    const auto& skeleton = *sceneData->getSkeleton();
    REQUIRE(skeleton.getJointCount() == 2);
    int rootIdx  = skeleton.findJointIndex("RootBone");
    int childIdx = skeleton.findJointIndex("ChildBone");
    REQUIRE(rootIdx >= 0);
    REQUIRE(childIdx >= 0);
    CHECK(skeleton.getJoint(static_cast<size_t>(rootIdx)).getParentIndex() == -1);
    CHECK(skeleton.getJoint(static_cast<size_t>(childIdx)).getParentIndex() == rootIdx);

    const auto& skinning = *sceneData->getSkinning();
    REQUIRE(skinning.getVertexCount() == 3);
    CHECK(skinning.getVertexWeights(0).getJointIndices().x == static_cast<uint32_t>(childIdx));
    CHECK(skinning.getVertexWeights(0).getWeights().x == Catch::Approx(1.0));

    const auto& clips = sceneData->getAnimationClips();
    REQUIRE(clips.size() == 1);
    CHECK(clips[0].getName() == "Lift");
    CHECK(clips[0].getDuration() == Catch::Approx(1.0));

    auto poseStart    = clips[0].evaluate(skeleton, 0.0, false);
    auto poseEnd      = clips[0].evaluate(skeleton, 1.0, false);
    auto skinMatStart = poseStart.computeSkinningMatrices(skeleton);
    auto skinMatEnd   = poseEnd.computeSkinningMatrices(skeleton);
    auto skinnedStart = skinning.computeSkinnedPositions(sceneData->getTriangulation()->vertices, skinMatStart);
    auto skinnedEnd   = skinning.computeSkinnedPositions(sceneData->getTriangulation()->vertices, skinMatEnd);

    CHECK(skinnedStart[0].y == Catch::Approx(0.0).margin(1e-4));
    CHECK(skinnedEnd[0].y == Catch::Approx(2.0).margin(1e-4));

    FbxAssetLoader loader;
    auto           loadedAsset = std::dynamic_pointer_cast<TriangulationAsset>(loader.load("RiggedTri", Polyglot::Node(bytes)));
    REQUIRE(loadedAsset != nullptr);
    REQUIRE(loadedAsset->getAttachments<SkeletonComponent>().size() == 1);
    CHECK(loadedAsset->getAttachments<SkeletonComponent>()[0]->getSkeleton().getJointCount() == 2);
    REQUIRE(loadedAsset->getAttachments<TriangulationSkinningComponent>().size() == 1);
    CHECK(loadedAsset->getAttachments<TriangulationSkinningComponent>()[0]->getSkinning().getVertexCount() == 3);
    REQUIRE(loadedAsset->getAttachments<AnimationComponent>().size() == 1);
    CHECK(loadedAsset->getAttachments<AnimationComponent>()[0]->getClips().size() == 1);
}
#endif
