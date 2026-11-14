#include "Database/Selection/SelectionEntry.h"

#include "Database/Selection/SubObjectSelection.h"
#include "Database/Selection/SubObjectSelectionFactory.h"

#include <string>
#include <utility>

SelectionEntry::SelectionEntry() {}

SelectionEntry::~SelectionEntry() {}

const CADacomb::GUID& SelectionEntry::getTargetGuid() const
{
    return targetGuid;
}

void SelectionEntry::setTargetGuid(const CADacomb::GUID& guid)
{
    targetGuid = guid;
}

std::vector<std::shared_ptr<const SubObjectSelection>> SelectionEntry::getSubObjects() const
{
    std::vector<std::shared_ptr<const SubObjectSelection>> result;
    result.reserve(subObjects.size());
    for (const auto& subObject : subObjects)
    {
        result.push_back(subObject);
    }
    return result;
}

void SelectionEntry::addSubObject(std::shared_ptr<SubObjectSelection> subObject)
{
    if (subObject)
    {
        subObjects.push_back(std::move(subObject));
    }
}

void SelectionEntry::removeSubObject(const std::string& type)
{
    for (auto it = subObjects.begin(); it != subObjects.end();)
    {
        if (*it && (*it)->getType() == type)
        {
            it = subObjects.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void SelectionEntry::clearSubObjects()
{
    subObjects.clear();
}

nlohmann::json SelectionEntry::toJson() const
{
    nlohmann::json subObjectsJson = nlohmann::json::array();
    for (const auto& subObject : subObjects)
    {
        if (subObject)
        {
            subObjectsJson.push_back(subObject->toJson());
        }
    }

    nlohmann::json jsonObject;
    jsonObject["Version"]    = 1;
    jsonObject["targetGuid"] = targetGuid.toString();
    jsonObject["subObjects"] = subObjectsJson;
    return jsonObject;
}

void SelectionEntry::fromJson(const nlohmann::json& jsonObject)
{
    int version = jsonObject.at("Version").get<int>();
    subObjects.clear();

    if (version == 1)
    {
        targetGuid = CADacomb::GUID(jsonObject.at("targetGuid").get<std::string>());
        for (const auto& item : jsonObject.at("subObjects"))
        {
            auto subObject = SubObjectSelectionFactory::fromJson(item);
            if (subObject)
            {
                subObjects.push_back(std::move(subObject));
            }
        }
    }
}

#ifdef ISTESTPROJECT
#include "Database/Selection/TriangleSubObjectSelection.h"
#include "Database/Selection/VoxelSubObjectSelection.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SelectionEntry/JsonRoundTripAndMultipleSubObjects")
{
    CADacomb::GUID guid = CADacomb::GUID::random();
    SelectionEntry entry;
    entry.setTargetGuid(guid);

    auto triSub = std::make_shared<TriangleSubObjectSelection>();
    triSub->add(5);
    entry.addSubObject(triSub);

    auto voxelSub = std::make_shared<VoxelSubObjectSelection>();
    voxelSub->add(10);
    voxelSub->add(11);
    entry.addSubObject(voxelSub);

    nlohmann::json serialized = entry.toJson();

    SelectionEntry restored;
    restored.fromJson(serialized);
    CHECK(restored.getTargetGuid() == guid);
    REQUIRE(restored.getSubObjects().size() == 2);
    CHECK(restored.getSubObjects()[0]->getType() == "Triangle");
    CHECK(restored.getSubObjects()[1]->getType() == "Voxel");

    restored.removeSubObject("Triangle");
    REQUIRE(restored.getSubObjects().size() == 1);
    CHECK(restored.getSubObjects()[0]->getType() == "Voxel");

    restored.clearSubObjects();
    CHECK(restored.getSubObjects().empty());
}
#endif
