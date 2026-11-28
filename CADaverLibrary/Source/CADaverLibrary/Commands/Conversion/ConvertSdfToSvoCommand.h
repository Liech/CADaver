#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <cstdint>
#include <memory>

class Asset;

// Command to convert an SdfAsset to a volumetric VoxelAsset (SVO).
class ConvertSdfToSvoCommand : public Command
{
  public:
    ConvertSdfToSvoCommand();
    virtual ~ConvertSdfToSvoCommand();

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
    uint32_t               dimension    = 64;
    bool                   deleteOrigin = false;
};
