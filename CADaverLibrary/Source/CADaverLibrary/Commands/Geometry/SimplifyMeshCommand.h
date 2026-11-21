#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>
#include <string>

class Asset;

// Command to simplify a triangulation mesh asset by percentage or target triangle count.
class SimplifyMeshCommand : public Command
{
  public:
    SimplifyMeshCommand();
    virtual ~SimplifyMeshCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual std::vector<CADacomb::GUID> getRequiredAssets() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    CADacomb::GUID         assetGuid;
    std::shared_ptr<Asset> originAsset;
    std::shared_ptr<Asset> createdAsset;
    std::string            mode                = "Percent";
    double                 targetPercent       = 50.0;
    size_t                 targetTriangleCount = 1000;
    bool                   deleteOrigin        = false;
};
