#include "Commands/CaptureScreenshotCommand.h"

#include "CADacombRender/Texture/TextureImageConverter.h"
#include "Database/Asset/TextureAsset.h"
#include "Database/Database.h"
#include "Database/Project.h"
#include "Rendering/Renderer.h"

CaptureScreenshotCommand::CaptureScreenshotCommand(Renderer& rendererInput)
  : renderer(rendererInput)
{
}

CaptureScreenshotCommand::~CaptureScreenshotCommand() {}

std::string CaptureScreenshotCommand::getType() const
{
    return "CaptureScreenshot";
}

int CaptureScreenshotCommand::getVersion() const
{
    return 1;
}

std::string CaptureScreenshotCommand::getDescription() const
{
    return "Command: CaptureScreenshot\n"
           "Version: 1\n"
           "Purpose: Captures the current viewport image from the renderer and adds it to the project as a TextureAsset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"CaptureScreenshot\",\n"
           "  \"version\": 1,\n"
           "  \"name\": <string> (optional, default: \"Screenshot\")\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"CaptureScreenshot\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"name\": <string>,\n"
           "  \"width\": <int>,\n"
           "  \"height\": <int>\n"
           "}";
}

void CaptureScreenshotCommand::setInput(const Polyglot::Node& node)
{
    assetName = "Screenshot";
    if (node.contains("name") && !node["name"].isNull())
    {
        assetName = node["name"].asString();
    }
}

nlohmann::json CaptureScreenshotCommand::execute(Database& database)
{
    auto texture = std::make_shared<CADacomb::Texture::Texture>(CADacombRender::TextureImageConverter::fromVulcantImage(renderer.getColor()));

    createdAsset = std::make_shared<TextureAsset>();
    createdAsset->setName(assetName);
    createdAsset->setTexture(texture);
    createdAssetGuid = createdAsset->getGuid();
    database.getProject().addAsset(createdAsset);

    return createResponse({
      { "assetGuid", createdAssetGuid.toString() },
      {      "name",                   assetName },
      {     "width",         texture->getWidth() },
      {    "height",        texture->getHeight() }
    });
}

bool CaptureScreenshotCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void CaptureScreenshotCommand::undo(Database& database)
{
    database.getProject().removeAsset(createdAssetGuid);
}

void CaptureScreenshotCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t CaptureScreenshotCommand::getUndoSize() const
{
    size_t size = sizeof(CaptureScreenshotCommand);
    if (createdAsset)
    {
        size += createdAsset->getSizeInBytes();
    }
    return size;
}
