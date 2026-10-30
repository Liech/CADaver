#include "Database/Asset/ProgressBarAsset.h"

#include "CADacomb/Util/ProgressBar.h"

#include <chrono>

ProgressBarAsset::ProgressBarAsset(const std::string& name, const CADacomb::GUID& taskGuid, const std::string& operationName, const std::vector<std::shared_ptr<Asset>>& abductedAssets)
  : Asset(name, "Progress")
  , taskGuid(taskGuid)
  , operationName(operationName)
  , abductedAssets(abductedAssets)
{
}

ProgressBarAsset::~ProgressBarAsset() {}

const CADacomb::GUID& ProgressBarAsset::getTaskGuid() const
{
    return taskGuid;
}

const std::string& ProgressBarAsset::getOperationName() const
{
    return operationName;
}

ProgressBarAssetState ProgressBarAsset::getState() const
{
    return state;
}

void ProgressBarAsset::setState(ProgressBarAssetState newState)
{
    state = newState;
}

float ProgressBarAsset::getProgress() const
{
    auto statuses = CADacomb::ProgressBar::getProgress(taskGuid);
    if (statuses.empty())
    {
        return 0.0f;
    }
    const auto& status = statuses.back();
    if (status.max == 0)
    {
        return 0.0f;
    }
    return static_cast<float>(status.current) / static_cast<float>(status.max);
}

const std::vector<std::shared_ptr<Asset>>& ProgressBarAsset::getAbductedAssets() const
{
    return abductedAssets;
}

std::shared_future<nlohmann::json> ProgressBarAsset::getFuture() const
{
    return future;
}

void ProgressBarAsset::setFuture(const std::shared_future<nlohmann::json>& newFuture)
{
    future = newFuture;
}

bool ProgressBarAsset::isReady() const
{
    if (!future.valid())
    {
        return false;
    }
    return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

nlohmann::json ProgressBarAsset::getResult() const
{
    if (isReady())
    {
        return future.get();
    }
    return {};
}

void ProgressBarAsset::cancel()
{
    CADacomb::ProgressBar::cancel(taskGuid);
    state = ProgressBarAssetState::Canceled;
}

size_t ProgressBarAsset::getSizeInBytes() const
{
    return sizeof(*this);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ProgressBarAsset/ConstructionAndState")
{
    CADacomb::GUID   taskGuid = CADacomb::GUID::random();
    ProgressBarAsset asset("Task", taskGuid, "Convert", {});

    CHECK(asset.getName() == "Task");
    CHECK(asset.getTaskGuid() == taskGuid);
    CHECK(asset.getOperationName() == "Convert");
    CHECK(asset.getState() == ProgressBarAssetState::Running);
    CHECK(asset.getType() == "Progress");
    CHECK_FALSE(asset.isReady());

    asset.cancel();
    CHECK(asset.getState() == ProgressBarAssetState::Canceled);
}
#endif
