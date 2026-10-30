#pragma once

#include "CADacomb/Util/GUID.h"
#include "Database/Asset.h"

#include <future>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

// Represents an in-progress asynchronous task replacing abducted assets in the project.
class ProgressBarAsset : public Asset
{
  public:
    ProgressBarAsset(const std::string& name, const CADacomb::GUID& taskGuid, const std::string& operationName, const std::vector<std::shared_ptr<Asset>>& abductedAssets);
    virtual ~ProgressBarAsset();

    const CADacomb::GUID& getTaskGuid() const;
    const std::string&    getOperationName() const;

    float getProgress() const;

    std::vector<std::shared_ptr<const Asset>> getAbductedAssets() const;

    std::shared_future<nlohmann::json> getFuture() const;
    void                               setFuture(const std::shared_future<nlohmann::json>& newFuture);
    bool                               isReady() const;
    nlohmann::json                     getResult() const;

    void cancel() const;

    virtual size_t getSizeInBytes() const override;

  private:
    CADacomb::GUID                      taskGuid;
    std::string                         operationName;
    std::vector<std::shared_ptr<Asset>> abductedAssets;
    std::shared_future<nlohmann::json>  future;
};
