#include "Commands/Component/AddTextureComponentCommand.h"

#include "CADacomb/Texture/Texture.h"
#include "Database/Asset.h"
#include "Database/Asset/TextureAsset.h"
#include "Database/Component/TextureComponent.h"
#include "Database/Database.h"
#include "Database/Project.h"
#include "Utility/IOValueGrabber.h"

#include <cstdint>
#include <exception>
#include <vector>

AddTextureComponentCommand::AddTextureComponentCommand() {}

AddTextureComponentCommand::~AddTextureComponentCommand() {}

std::string AddTextureComponentCommand::getType() const
{
    return "AddTextureComponent";
}

int AddTextureComponentCommand::getVersion() const
{
    return 1;
}

std::string AddTextureComponentCommand::getDescription() const
{
    return "Command: AddTextureComponent\n"
           "Version: 1\n"
           "Purpose: Attaches a TextureComponent to a compatible asset (e.g. Triangulation) using either an existing TextureAsset or an image file/memory source (PNG, JPG, BMP, TGA).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"AddTextureComponent\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": \"<string>\",\n"
           "  \"textureAssetGuid\": \"<string, optional>\",\n"
           "  \"source\": <file path string, binary, or callback, optional>,\n"
           "  \"name\": \"<string, optional>\"\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"AddTextureComponent\",\n"
           "  \"version\": 1,\n"
           "  \"componentGuid\": \"<string>\"\n"
           "}";
}

void AddTextureComponentCommand::setInput(const Polyglot::Node& node)
{
    assetGuid           = CADacomb::GUID(node["assetGuid"].asString());
    hasTextureAssetGuid = node.contains("textureAssetGuid") && node["textureAssetGuid"].isString() && !node["textureAssetGuid"].asString().empty();
    if (hasTextureAssetGuid)
    {
        textureAssetGuid = CADacomb::GUID(node["textureAssetGuid"].asString());
    }

    hasSource = node.contains("source") && !node["source"].isNull();
    if (hasSource)
    {
        source = node["source"];
    }

    name = node.contains("name") && node["name"].isString() ? node["name"].asString() : "";
}

nlohmann::json AddTextureComponentCommand::execute(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    auto component = std::make_shared<TextureComponent>();
    if (!component->isCompatible(asset->getType()))
    {
        return createErrorResponse("IncompatibleComponent", "Texture component is incompatible with asset type " + asset->getType());
    }

    std::shared_ptr<CADacomb::Texture::Texture> resolvedTexture;
    std::string                                 resolvedName = name;

    if (hasTextureAssetGuid)
    {
        auto textureAsset = std::dynamic_pointer_cast<TextureAsset>(database.getProject().getAsset(textureAssetGuid));
        if (!textureAsset)
        {
            return createErrorResponse("TextureAssetNotFound", "Texture asset not found: " + textureAssetGuid.toString());
        }
        resolvedTexture = std::make_shared<CADacomb::Texture::Texture>(textureAsset->getTexture());
        if (resolvedName.empty())
        {
            resolvedName = textureAsset->getName();
        }
    }
    else if (hasSource)
    {
        std::vector<uint8_t> bytes = IOValueGrabber::grabBytes(source);
        if (bytes.empty())
        {
            return createErrorResponse("TextureLoadFailed", "Failed to read texture source bytes");
        }

        try
        {
            resolvedTexture = std::make_shared<CADacomb::Texture::Texture>(CADacomb::Texture::Texture::loadFromMemory(bytes));
        }
        catch (const std::exception& ex)
        {
            return createErrorResponse("TextureLoadFailed", std::string("Failed to decode texture: ") + ex.what());
        }
    }
    else
    {
        return createErrorResponse("MissingTextureSource", "Provide either textureAssetGuid or source");
    }

    component->setTexture(resolvedTexture);
    component->setName(resolvedName.empty() ? "Texture" : resolvedName);

    addedComponent          = component;
    CADacomb::GUID compGuid = asset->addComponent(addedComponent);
    return createResponse({
      { "componentGuid", compGuid.toString() }
    });
}

bool AddTextureComponentCommand::isUndoable() const
{
    return addedComponent != nullptr;
}

void AddTextureComponentCommand::undo(Database& database)
{
    if (addedComponent)
    {
        auto asset = database.getProject().getAsset(assetGuid);
        if (asset)
        {
            asset->removeComponent(addedComponent->getGuid());
        }
    }
}

void AddTextureComponentCommand::redo(Database& database)
{
    if (addedComponent)
    {
        auto asset = database.getProject().getAsset(assetGuid);
        if (asset)
        {
            asset->addComponent(addedComponent);
        }
    }
}

size_t AddTextureComponentCommand::getUndoSize() const
{
    return sizeof(AddTextureComponentCommand) + (addedComponent ? addedComponent->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("AddTextureComponentCommand/AttachesFromTextureAssetAndSupportsUndoRedo")
{
    Database db;
    auto     triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setName("Mesh");
    db.getProject().addAsset(triAsset);

    auto texData = std::make_shared<CADacomb::Texture::Texture>(2, 2, glm::u8vec4(10, 20, 30, 255));
    texData->setPixel(1, 0, glm::u8vec4(240, 120, 60, 255));
    auto texAsset = std::make_shared<TextureAsset>();
    texAsset->setName("Albedo");
    texAsset->setTexture(texData);
    db.getProject().addAsset(texAsset);

    AddTextureComponentCommand cmd;
    cmd.setInput(Polyglot::Node(nlohmann::json{
      {             "type",          "AddTextureComponent" },
      {          "version",                              1 },
      {        "assetGuid", triAsset->getGuid().toString() },
      { "textureAssetGuid", texAsset->getGuid().toString() }
    }));

    nlohmann::json result = cmd.execute(db);
    REQUIRE(result["type"] == "AddTextureComponent");
    REQUIRE(triAsset->getAttachments<TextureComponent>().size() == 1);
    CHECK(triAsset->getAttachments<TextureComponent>()[0]->getName() == "Albedo");
    CHECK(triAsset->getAttachments<TextureComponent>()[0]->getTexture().getPixel(1, 0) == glm::u8vec4(240, 120, 60, 255));

    cmd.undo(db);
    CHECK(triAsset->getAttachments<TextureComponent>().empty());

    cmd.redo(db);
    REQUIRE(triAsset->getAttachments<TextureComponent>().size() == 1);
}

TEST_CASE("AddTextureComponentCommand/AttachesFromImageSourceBytes")
{
    Database db;
    auto     triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setName("Mesh");
    db.getProject().addAsset(triAsset);

    CADacomb::Texture::Texture srcTex(2, 2, glm::u8vec4(5, 15, 25, 255));
    srcTex.setPixel(0, 1, glm::u8vec4(11, 22, 33, 255));
    std::vector<uint8_t> pngBytes = srcTex.encodeToPngMemory();

    Polyglot::Node input = {
        {      "type",          "AddTextureComponent" },
        {   "version",                              1 },
        { "assetGuid", triAsset->getGuid().toString() },
        {      "name",                    "CustomTex" },
        {    "source",                       pngBytes }
    };

    AddTextureComponentCommand cmd;
    cmd.setInput(input);
    nlohmann::json result = cmd.execute(db);
    REQUIRE(result["type"] == "AddTextureComponent");

    auto textures = triAsset->getAttachments<TextureComponent>();
    REQUIRE(textures.size() == 1);
    CHECK(textures[0]->getName() == "CustomTex");
    CHECK(textures[0]->getTexture().getPixel(0, 1) == glm::u8vec4(11, 22, 33, 255));
}

TEST_CASE("AddTextureComponentCommand/RejectsIncompatibleAssetType")
{
    Database db;
    auto     voxelAsset = std::make_shared<VoxelAsset>();
    voxelAsset->setName("Voxels");
    db.getProject().addAsset(voxelAsset);

    AddTextureComponentCommand cmd;
    cmd.setInput(Polyglot::Node(nlohmann::json{
      {      "type",            "AddTextureComponent" },
      {   "version",                                1 },
      { "assetGuid", voxelAsset->getGuid().toString() }
    }));

    nlohmann::json result = cmd.execute(db);
    CHECK(result["type"] == "Error");
    CHECK(result["errorType"] == "IncompatibleComponent");
}
#endif
