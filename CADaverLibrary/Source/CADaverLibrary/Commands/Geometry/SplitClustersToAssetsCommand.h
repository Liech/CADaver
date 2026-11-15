#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>
#include <string>
#include <vector>

class TriangulationAsset;

// Splits a TriangulationAsset's FaceValuesComponent clustering into separate TriangulationAssets.
class SplitClustersToAssetsCommand : public Command
{
  public:
    SplitClustersToAssetsCommand();
    virtual ~SplitClustersToAssetsCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    std::vector<CADacomb::GUID> getRequiredAssets() const override;

    void           setInput(const Polyglot::Node& node) override;
    nlohmann::json execute(Database& database) override;

    bool   isUndoable() const override;
    void   undo(Database& database) override;
    void   redo(Database& database) override;
    size_t getUndoSize() const override;

  private:
    CADacomb::GUID                                   assetGuid;
    CADacomb::GUID                                   componentGuid;
    std::vector<std::shared_ptr<TriangulationAsset>> createdAssets;
};
