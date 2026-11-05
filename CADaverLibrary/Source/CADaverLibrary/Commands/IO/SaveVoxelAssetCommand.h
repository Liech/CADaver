#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"
#include "Polyglot/Node.h"

#include <string>
#include <vector>

// Command to save an SVO voxel asset.
class SaveVoxelAssetCommand : public Command
{
  public:
    SaveVoxelAssetCommand();
    virtual ~SaveVoxelAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual std::vector<CADacomb::GUID> getRequiredAssets() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    CADacomb::GUID assetGuid;
    Polyglot::Node destination;
};
