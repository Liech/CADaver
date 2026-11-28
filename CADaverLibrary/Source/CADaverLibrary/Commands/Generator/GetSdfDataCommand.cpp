#include "Commands/Generator/GetSdfDataCommand.h"

#include "Database/Asset/SdfAsset.h"
#include "Database/Database.h"

GetSdfDataCommand::GetSdfDataCommand() {}

GetSdfDataCommand::~GetSdfDataCommand() {}

std::string GetSdfDataCommand::getType() const
{
    return "GetSdfData";
}

int GetSdfDataCommand::getVersion() const
{
    return 1;
}

std::string GetSdfDataCommand::getDescription() const
{
    return "Command: GetSdfData\n"
           "Version: 1\n"
           "Purpose: Returns the Sdf JSON data (Formula and AABB bounds) of an existing SdfAsset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetSdfData\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetSdfData\",\n"
           "  \"version\": 1,\n"
           "  \"sdf\": <Sdf JSON object>\n"
           "}";
}

void GetSdfDataCommand::setInput(const Polyglot::Node& node)
{
    assetGuid = node["assetGuid"].asString();
}

nlohmann::json GetSdfDataCommand::execute(Database& database)
{
    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    auto sdfAsset = std::dynamic_pointer_cast<SdfAsset>(baseAsset);
    if (!sdfAsset)
    {
        return createErrorResponse("NotSdfAsset", "Asset is not an SdfAsset");
    }

    return createResponse({
      { "sdf", sdfAsset->getSdf().toJson() }
    });
}
