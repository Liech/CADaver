#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <glm/glm.hpp>

// Command to set the absolute transformation matrix of an asset.
class TransformAssetCommand : public Command
{
  public:
    TransformAssetCommand();
    virtual ~TransformAssetCommand();

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
    CADacomb::GUID assetGuid;
    glm::dmat4     transformation{ 1.0 };
    glm::dmat4     previousTransformation{ 1.0 };
};
