#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <cstdint>
#include <memory>

class Asset;

// Command to convert an SdfAsset to a TriangulationAsset via SurfaceNets.
class ConvertSdfToStlCommand : public Command
{
  public:
    ConvertSdfToStlCommand();
    virtual ~ConvertSdfToStlCommand();

    virtual std::string                 getType() const override;
    virtual int                         getVersion() const override;
    virtual std::string                 getDescription() const override;
    virtual std::vector<CADacomb::GUID> getRequiredAssets() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    std::shared_ptr<Asset> createdAsset;
    std::shared_ptr<Asset> originAsset;
    CADacomb::GUID         assetGuid;
    uint32_t               resolution   = 64;
    bool                   deleteOrigin = false;
};
