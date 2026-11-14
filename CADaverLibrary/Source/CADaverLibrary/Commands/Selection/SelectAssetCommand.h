#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <nlohmann/json.hpp>
#include <string>

// Command to select an asset or subasset (with optional subobject selection) in the database.
class SelectAssetCommand : public Command
{
  public:
    SelectAssetCommand();
    virtual ~SelectAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    CADacomb::GUID assetGuid;
    std::string    mode = "Replace";
    nlohmann::json subObjectsJson;
};
