#include "Database/Selection/Selection.h"

Selection::Selection() {}

Selection::~Selection() {}

void Selection::select(const SelectionEntry& entry)
{
    entries.clear();
    entries.emplace(entry.getTargetGuid(), entry);
}

void Selection::addSelection(const SelectionEntry& entry)
{
    entries[entry.getTargetGuid()] = entry;
}

void Selection::deselect(const CADacomb::GUID& targetGuid)
{
    entries.erase(targetGuid);
}

void Selection::clear()
{
    entries.clear();
}

bool Selection::isSelected(const CADacomb::GUID& targetGuid) const
{
    return entries.find(targetGuid) != entries.end();
}

std::vector<SelectionEntry> Selection::getEntries() const
{
    std::vector<SelectionEntry> result;
    result.reserve(entries.size());
    for (const auto& pair : entries)
    {
        result.push_back(pair.second);
    }
    return result;
}

nlohmann::json Selection::toJson() const
{
    nlohmann::json entriesJson = nlohmann::json::array();
    for (const auto& pair : entries)
    {
        entriesJson.push_back(pair.second.toJson());
    }

    nlohmann::json jsonObject;
    jsonObject["Version"] = 1;
    jsonObject["entries"] = entriesJson;
    return jsonObject;
}

void Selection::fromJson(const nlohmann::json& jsonObject)
{
    entries.clear();
    int version = jsonObject.at("Version").get<int>();
    if (version == 1)
    {
        for (const auto& item : jsonObject.at("entries"))
        {
            SelectionEntry entry;
            entry.fromJson(item);
            entries.emplace(entry.getTargetGuid(), std::move(entry));
        }
    }
}

#ifdef ISTESTPROJECT
#include "Database/Selection/VoxelSubObjectSelection.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Selection/SelectAddDeselectAndJson")
{
    Selection      selection;
    CADacomb::GUID guidA = CADacomb::GUID::random();
    CADacomb::GUID guidB = CADacomb::GUID::random();

    CHECK(selection.getEntries().empty());

    SelectionEntry entryA;
    entryA.setTargetGuid(guidA);
    selection.select(entryA);
    CHECK(selection.getEntries().size() == 1);
    CHECK(selection.isSelected(guidA));

    auto voxelSub = std::make_shared<VoxelSubObjectSelection>();
    voxelSub->add(12);
    SelectionEntry entryB;
    entryB.setTargetGuid(guidB);
    entryB.addSubObject(voxelSub);
    selection.addSelection(entryB);
    CHECK(selection.getEntries().size() == 2);
    CHECK(selection.isSelected(guidB));

    nlohmann::json serialized = selection.toJson();

    Selection restored;
    restored.fromJson(serialized);
    CHECK(restored.getEntries().size() == 2);
    CHECK(restored.isSelected(guidA));
    CHECK(restored.isSelected(guidB));

    bool foundVoxelSub = false;
    for (const auto& entry : restored.getEntries())
    {
        if (entry.getTargetGuid() == guidB)
        {
            REQUIRE(entry.getSubObjects().size() == 1);
            CHECK(entry.getSubObjects()[0]->getType() == "Voxel");
            foundVoxelSub = true;
        }
    }
    CHECK(foundVoxelSub);

    restored.deselect(guidA);
    CHECK(restored.getEntries().size() == 1);
    CHECK(!restored.isSelected(guidA));

    restored.clear();
    CHECK(restored.getEntries().empty());
}
#endif
