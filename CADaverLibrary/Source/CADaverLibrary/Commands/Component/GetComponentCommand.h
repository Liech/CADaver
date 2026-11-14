#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <string>

// Generic command to query one or all AssetComponents attached to an asset.
class GetComponentCommand : public Command
{
  public:
    GetComponentCommand();
    virtual ~GetComponentCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    void           setInput(const Polyglot::Node& node) override;
    nlohmann::json execute(Database& database) override;

  private:
    CADacomb::GUID assetGuid;
    CADacomb::GUID componentGuid;
    std::string    componentType;
};
