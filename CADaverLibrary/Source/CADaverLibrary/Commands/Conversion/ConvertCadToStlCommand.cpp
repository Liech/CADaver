#include "Commands/Conversion/ConvertCadToStlCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Conversion/AssetConverter.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"

ConvertCadToStlCommand::ConvertCadToStlCommand() {}

ConvertCadToStlCommand::~ConvertCadToStlCommand() {}

std::string ConvertCadToStlCommand::getType() const
{
    return "ConvertCadToStl";
}

int ConvertCadToStlCommand::getVersion() const
{
    return 1;
}

std::string ConvertCadToStlCommand::getDescription() const
{
    return "Command: ConvertCadToStl\n"
           "Version: 1\n"
           "Purpose: Converts a CAD asset to a triangulation STL asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertCadToStl\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"precision\": <double> (optional, default: 0.1),\n"
           "  \"deleteOrigin\": <bool> (optional, default: false)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertCadToStl\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotCadAsset\" | \"ConversionFailed\",\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> ConvertCadToStlCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ConvertCadToStlCommand::setInput(const nlohmann::json& json)
{
    assetGuid    = json.value("assetGuid", "");
    precision    = json.value("precision", 0.1);
    deleteOrigin = json.value("deleteOrigin", false);
}

nlohmann::json ConvertCadToStlCommand::execute(Database& database, std::istream*, std::ostream*)
{
    CADacomb::ProgressBar bar(1, "Converting CAD to STL");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createResponse("AssetNotFound",
                              {
                                { "assetGuid", "" }
        });
    }

    auto cadAsset = std::dynamic_pointer_cast<CadAsset>(baseAsset);
    if (!cadAsset || bar.isCanceled())
    {
        return createResponse("NotCadAsset",
                              {
                                { "assetGuid", "" }
        });
    }

    auto stlAsset = AssetConverter::cadToStl(*cadAsset, precision);
    if (!stlAsset || bar.isCanceled())
    {
        return createResponse("ConversionFailed",
                              {
                                { "assetGuid", "" }
        });
    }

    bar.addProgress(1);

    database.getProject().addAsset(stlAsset);
    createdAsset = stlAsset;

    if (deleteOrigin)
    {
        originAsset = baseAsset;
        database.getProject().removeAsset(assetGuid);
    }

    return createResponse("Success",
                          {
                            { "assetGuid", stlAsset->getGuid().toString() }
    });
}

bool ConvertCadToStlCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void ConvertCadToStlCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
    if (deleteOrigin && originAsset)
    {
        database.getProject().addAsset(originAsset);
    }
}

void ConvertCadToStlCommand::redo(Database& database)
{
    if (deleteOrigin && originAsset)
    {
        database.getProject().removeAsset(originAsset->getGuid());
    }
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t ConvertCadToStlCommand::getUndoSize() const
{
    size_t size = sizeof(ConvertCadToStlCommand);
    if (originAsset)
    {
        size += originAsset->getSizeInBytes();
    }
    if (createdAsset)
    {
        size += createdAsset->getSizeInBytes();
    }
    return size;
}
