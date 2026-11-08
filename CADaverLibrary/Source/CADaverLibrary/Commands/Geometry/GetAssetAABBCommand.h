#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Universal command to query the axis-aligned bounding box (AABB) of any geometry asset.
class GetAssetAABBCommand : public Command
{
  public:
    GetAssetAABBCommand();
    virtual ~GetAssetAABBCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    CADacomb::GUID assetGuid;
    std::string    assetName;
};
