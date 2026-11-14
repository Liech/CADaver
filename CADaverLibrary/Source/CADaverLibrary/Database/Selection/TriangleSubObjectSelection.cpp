#include "Database/Selection/TriangleSubObjectSelection.h"

TriangleSubObjectSelection::TriangleSubObjectSelection() {}

TriangleSubObjectSelection::~TriangleSubObjectSelection() {}

std::string TriangleSubObjectSelection::getType() const
{
    return "Triangle";
}

nlohmann::json TriangleSubObjectSelection::toJson() const
{
    nlohmann::json indicesJson = nlohmann::json::array();
    for (size_t index : triangleIndices)
    {
        indicesJson.push_back(index);
    }

    nlohmann::json jsonObject;
    jsonObject["Version"] = 1;
    jsonObject["type"]    = getType();
    jsonObject["indices"] = indicesJson;
    return jsonObject;
}

void TriangleSubObjectSelection::fromJson(const nlohmann::json& jsonObject)
{
    triangleIndices.clear();
    int version = jsonObject.at("Version").get<int>();
    if (version == 1)
    {
        for (const auto& item : jsonObject.at("indices"))
        {
            triangleIndices.insert(item.get<size_t>());
        }
    }
}

void TriangleSubObjectSelection::add(size_t index)
{
    triangleIndices.insert(index);
}

void TriangleSubObjectSelection::remove(size_t index)
{
    triangleIndices.erase(index);
}

void TriangleSubObjectSelection::clear()
{
    triangleIndices.clear();
}

bool TriangleSubObjectSelection::contains(size_t index) const
{
    return triangleIndices.find(index) != triangleIndices.end();
}

std::set<size_t> TriangleSubObjectSelection::getIndices() const
{
    return std::set<size_t>(triangleIndices.begin(), triangleIndices.end());
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TriangleSubObjectSelection/JsonRoundTrip")
{
    TriangleSubObjectSelection original;
    original.add(3);
    original.add(7);
    original.add(3);

    CHECK(original.getType() == "Triangle");
    CHECK(original.contains(3));
    CHECK(original.contains(7));
    CHECK(original.getIndices().size() == 2);

    nlohmann::json serialized = original.toJson();

    TriangleSubObjectSelection restored;
    restored.fromJson(serialized);
    CHECK(restored.getIndices() == std::set<size_t>{ 3, 7 });

    restored.remove(3);
    CHECK(!restored.contains(3));
    CHECK(restored.contains(7));

    restored.clear();
    CHECK(restored.getIndices().empty());
}
#endif
