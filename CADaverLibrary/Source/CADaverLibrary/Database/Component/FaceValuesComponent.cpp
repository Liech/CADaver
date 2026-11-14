#include "Database/Component/FaceValuesComponent.h"

FaceValuesComponent::FaceValuesComponent()
  : metadata(nlohmann::json::object())
  , luaScript(getDefaultLuaScript())
{
}

FaceValuesComponent::~FaceValuesComponent() {}

std::string FaceValuesComponent::getType() const
{
    return "FaceValues";
}

std::string FaceValuesComponent::getDescription() const
{
    return "Component: FaceValues\n"
           "Purpose: Assigns indexed JSON values to triangulation faces and maps entries to RGB colors via a Lua getColor(entry, meta) function.\n"
           "Compatibility: Compatible only with Triangulation assets.\n\n"
           "Data Members:\n"
           "  - values: Array of unique JSON entries\n"
           "  - faceValueIndices: Array of uint32 indices (one per triangle face) into values\n"
           "  - metadata: Component-level JSON object passed as meta to Lua\n"
           "  - luaScript: Lua script defining function getColor(entry, meta) returning {r, g, b}";
}

size_t FaceValuesComponent::getSizeInBytes() const
{
    size_t total = sizeof(FaceValuesComponent) + luaScript.capacity();
    total += faceValueIndices.capacity() * sizeof(uint32_t);
    total += values.capacity() * sizeof(nlohmann::json);
    for (const auto& entry : values)
    {
        total += entry.dump().size();
    }
    total += metadata.dump().size();
    return total;
}

bool FaceValuesComponent::isCompatible(const std::string& assetType) const
{
    return assetType == "Triangulation";
}

nlohmann::json FaceValuesComponent::toDataJson() const
{
    return {
        {          "Version",                1 },
        {           "values",           values },
        { "faceValueIndices", faceValueIndices },
        {         "metadata",         metadata },
        {        "luaScript",        luaScript }
    };
}

void FaceValuesComponent::fromDataJson(const nlohmann::json& jsonObject)
{
    jsonObject.at("Version").get<int>();
    values           = jsonObject.at("values").get<std::vector<nlohmann::json>>();
    faceValueIndices = jsonObject.at("faceValueIndices").get<std::vector<uint32_t>>();
    metadata         = jsonObject.at("metadata");
    luaScript        = jsonObject.at("luaScript").get<std::string>();
    markModified();
}

const std::vector<nlohmann::json>& FaceValuesComponent::getValues() const
{
    return values;
}

void FaceValuesComponent::setValues(const std::vector<nlohmann::json>& newValues)
{
    values = newValues;
    markModified();
}

const std::vector<uint32_t>& FaceValuesComponent::getFaceValueIndices() const
{
    return faceValueIndices;
}

void FaceValuesComponent::setFaceValueIndices(const std::vector<uint32_t>& newFaceValueIndices)
{
    faceValueIndices = newFaceValueIndices;
    markModified();
}

const nlohmann::json& FaceValuesComponent::getMetadata() const
{
    return metadata;
}

void FaceValuesComponent::setMetadata(const nlohmann::json& newMetadata)
{
    metadata = newMetadata;
    markModified();
}

const std::string& FaceValuesComponent::getLuaScript() const
{
    return luaScript;
}

void FaceValuesComponent::setLuaScript(const std::string& newLuaScript)
{
    luaScript = newLuaScript;
    markModified();
}

std::string FaceValuesComponent::getDefaultLuaScript()
{
    return "function getColor(entry, meta)\n"
           "    local palette = {\n"
           "        { 0.90, 0.25, 0.25 },\n"
           "        { 0.25, 0.75, 0.35 },\n"
           "        { 0.25, 0.50, 0.90 },\n"
           "        { 0.95, 0.75, 0.20 },\n"
           "        { 0.70, 0.35, 0.85 },\n"
           "        { 0.20, 0.80, 0.80 },\n"
           "        { 0.95, 0.50, 0.20 },\n"
           "        { 0.60, 0.60, 0.60 }\n"
           "    }\n"
           "    local id = 0\n"
           "    if type(entry) == \"number\" then\n"
           "        id = math.floor(entry)\n"
           "    elseif type(entry) == \"table\" then\n"
           "        if type(entry.cluster) == \"number\" then\n"
           "            id = math.floor(entry.cluster)\n"
           "        elseif type(entry.id) == \"number\" then\n"
           "            id = math.floor(entry.id)\n"
           "        elseif type(entry.label) == \"number\" then\n"
           "            id = math.floor(entry.label)\n"
           "        elseif type(entry.r) == \"number\" and type(entry.g) == \"number\" and type(entry.b) == \"number\" then\n"
           "            return { entry.r, entry.g, entry.b }\n"
           "        end\n"
           "    end\n"
           "    local idx = (math.abs(id) % #palette) + 1\n"
           "    return palette[idx]\n"
           "end\n";
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FaceValuesComponent/CompatibilityAndSerialization")
{
    FaceValuesComponent comp;
    CHECK(comp.getType() == "FaceValues");
    CHECK(comp.isCompatible("Triangulation") == true);
    CHECK(comp.isCompatible("CAD") == false);
    CHECK(comp.isCompatible("Voxel") == false);
    CHECK(!comp.getLuaScript().empty());

    comp.setName("Clusters");
    comp.setValues({
      { { "cluster", 0 } },
      { { "cluster", 1 } }
    });
    comp.setFaceValueIndices({ 0, 1, 0 });
    comp.setMetadata({
      { "clusterCount", 2 }
    });

    nlohmann::json serialized = comp.toJson();

    FaceValuesComponent restored;
    restored.fromJson(serialized);
    CHECK(restored.getName() == "Clusters");
    REQUIRE(restored.getValues().size() == 2);
    CHECK(restored.getValues()[1].at("cluster").get<int>() == 1);
    CHECK(restored.getFaceValueIndices() == std::vector<uint32_t>{ 0, 1, 0 });
    CHECK(restored.getMetadata().at("clusterCount").get<int>() == 2);
    CHECK(restored.getSizeInBytes() >= sizeof(FaceValuesComponent));
}
#endif
