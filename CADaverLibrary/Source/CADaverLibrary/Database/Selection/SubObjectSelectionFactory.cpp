#include "Database/Selection/SubObjectSelectionFactory.h"

#include "Database/Selection/SubObjectSelection.h"
#include "Database/Selection/TriangleSubObjectSelection.h"
#include "Database/Selection/VoxelSubObjectSelection.h"

#include <functional>
#include <map>
#include <string>

SubObjectSelectionFactory::SubObjectSelectionFactory() {}

SubObjectSelectionFactory::~SubObjectSelectionFactory() {}

std::shared_ptr<SubObjectSelection> SubObjectSelectionFactory::fromJson(const nlohmann::json& jsonObject)
{
    static const std::map<std::string, std::function<std::shared_ptr<SubObjectSelection>()>> creators = {
        { "Triangle", []() { return std::make_shared<TriangleSubObjectSelection>(); } },
        {    "Voxel",    []() { return std::make_shared<VoxelSubObjectSelection>(); } }
    };

    std::string type = jsonObject.at("type").get<std::string>();
    auto        it   = creators.find(type);
    if (it == creators.end())
    {
        return nullptr;
    }

    std::shared_ptr<SubObjectSelection> instance = it->second();
    instance->fromJson(jsonObject);
    return instance;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SubObjectSelectionFactory/Deserialize")
{
    nlohmann::json triJson = {
        { "Version",          1 },
        {    "type", "Triangle" },
        { "indices",   { 1, 2 } }
    };
    auto tri = SubObjectSelectionFactory::fromJson(triJson);
    REQUIRE(tri != nullptr);
    CHECK(tri->getType() == "Triangle");

    nlohmann::json unknownJson = {
        { "Version",         1 },
        {    "type", "Unknown" }
    };
    CHECK(SubObjectSelectionFactory::fromJson(unknownJson) == nullptr);
}
#endif
