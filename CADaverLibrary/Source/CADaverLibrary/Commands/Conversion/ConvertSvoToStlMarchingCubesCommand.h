#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>

class Asset;

// Command to convert a volumetric SVO asset to a Marching Cubes triangulation STL asset.
class ConvertSvoToStlMarchingCubesCommand : public Command
{
  public:
    ConvertSvoToStlMarchingCubesCommand();
    virtual ~ConvertSvoToStlMarchingCubesCommand();

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
    int                    targetDepth  = -1;
    bool                   deleteOrigin = false;
};
