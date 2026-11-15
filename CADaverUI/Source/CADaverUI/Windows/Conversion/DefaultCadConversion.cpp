#include "Windows/Conversion/DefaultCadConversion.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <chrono>
#include <nlohmann/json.hpp>

DefaultCadConversion::DefaultCadConversion(GatedDatabase& gatedDatabase, Defer& defer, const CADacomb::GUID& assetGuid) : gatedDatabase(&gatedDatabase), defer(&defer), assetGuid(assetGuid) {}

DefaultCadConversion::~DefaultCadConversion() {}

void DefaultCadConversion::start(GatedDatabase& gatedDatabase, Defer& defer, const CADacomb::GUID& assetGuid)
{
    std::make_shared<DefaultCadConversion>(gatedDatabase, defer, assetGuid)->launch();
}

void DefaultCadConversion::launch()
{
    clusterFuture = gatedDatabase->executeAsync({
      {      "type", "ClusterByNormal" },
      {   "version",                 1 },
      { "assetGuid", assetGuid.toString() }
    });
    schedulePoll();
}

void DefaultCadConversion::schedulePoll()
{
    std::shared_ptr<DefaultCadConversion> self = shared_from_this();
    defer->defer([self]() { self->poll(); });
}

void DefaultCadConversion::poll()
{
    if (clusterFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready || gatedDatabase->hasActiveTasks())
    {
        schedulePoll();
        return;
    }

    nlohmann::json result = clusterFuture.get().toJson();
    if (result.at("type").get<std::string>() == "Error")
    {
        return;
    }

    gatedDatabase->executeAsync({
      {          "type", "ConvertClusteringToCad" },
      {       "version",                        1 },
      {     "assetGuid",   assetGuid.toString() },
      { "componentGuid", result.at("componentGuid").get<std::string>() }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>
#include <thread>

TEST_CASE("DefaultCadConversion/ClustersAndConvertsCube")
{
    GatedDatabase gatedDatabase;
    Defer         deferQueue;
    gatedDatabase.setDispatchCallback([&deferQueue](std::function<void()> task) { deferQueue.defer(std::move(task)); });

    Polyglot::Node generated = gatedDatabase.execute({
      {    "type", "GenerateBaseShape" },
      { "version",                   1 },
      {   "shape",              "cube" }
    });
    CADacomb::GUID assetGuid(generated.toJson().at("assetGuid").get<std::string>());

    DefaultCadConversion::start(gatedDatabase, deferQueue, assetGuid);

    bool found = false;
    for (int i = 0; i < 2000 && !found; i++)
    {
        deferQueue.process();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        nlohmann::json assets = gatedDatabase.execute({
                                                        { "type", "GetAssets" },
                                                        { "version", 1 }
        })
                                  .toJson();
        for (const auto& asset : assets.at("assets"))
        {
            if (asset.at("name").get<std::string>() == "cube_CAD" && asset.at("type").get<std::string>() != "ProgressBar")
            {
                found = true;
            }
        }
    }
    CHECK(found);
}
#endif
