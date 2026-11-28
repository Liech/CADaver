#pragma once

#include "CADacomb/SDF/Sdf.h"
#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Updates an existing SdfAsset's Formula and AABB with undo/redo support.
class UpdateSdfAssetCommand : public Command
{
  public:
    UpdateSdfAssetCommand();
    virtual ~UpdateSdfAssetCommand();

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
    CADacomb::GUID assetGuid;
    CADacomb::Sdf  previousSdf;
    CADacomb::Sdf  newSdf;
    bool           executed = false;
};
