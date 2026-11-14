#include "Database/Selection/VoxelSubObjectSelection.h"

VoxelSubObjectSelection::VoxelSubObjectSelection() {}

VoxelSubObjectSelection::~VoxelSubObjectSelection() {}

std::string VoxelSubObjectSelection::getType() const
{
    return "Voxel";
}

nlohmann::json VoxelSubObjectSelection::toJson() const
{
    nlohmann::json indicesJson = nlohmann::json::array();
    for (size_t index : voxelIndices)
    {
        indicesJson.push_back(index);
    }

    nlohmann::json jsonObject;
    jsonObject["Version"] = 1;
    jsonObject["type"]    = getType();
    jsonObject["indices"] = indicesJson;
    return jsonObject;
}

void VoxelSubObjectSelection::fromJson(const nlohmann::json& jsonObject)
{
    voxelIndices.clear();
    int version = jsonObject.at("Version").get<int>();
    if (version == 1)
    {
        for (const auto& item : jsonObject.at("indices"))
        {
            voxelIndices.insert(item.get<size_t>());
        }
    }
}

void VoxelSubObjectSelection::add(size_t index)
{
    voxelIndices.insert(index);
}

void VoxelSubObjectSelection::remove(size_t index)
{
    voxelIndices.erase(index);
}

void VoxelSubObjectSelection::clear()
{
    voxelIndices.clear();
}

bool VoxelSubObjectSelection::contains(size_t index) const
{
    return voxelIndices.find(index) != voxelIndices.end();
}

std::set<size_t> VoxelSubObjectSelection::getIndices() const
{
    return std::set<size_t>(voxelIndices.begin(), voxelIndices.end());
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("VoxelSubObjectSelection/JsonRoundTrip")
{
    VoxelSubObjectSelection original;
    original.add(10);
    original.add(42);

    CHECK(original.getType() == "Voxel");
    CHECK(original.contains(10));
    CHECK(original.contains(42));
    CHECK(original.getIndices().size() == 2);

    nlohmann::json serialized = original.toJson();

    VoxelSubObjectSelection restored;
    restored.fromJson(serialized);
    CHECK(restored.getIndices() == std::set<size_t>{ 10, 42 });

    restored.remove(10);
    CHECK(!restored.contains(10));
    CHECK(restored.contains(42));

    restored.clear();
    CHECK(restored.getIndices().empty());
}
#endif
