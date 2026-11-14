#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <nlohmann/json.hpp>
#include <string>

// Generic command to update any AssetComponent's name or data payload.
class UpdateComponentCommand : public Command
{
  public:
    UpdateComponentCommand();
    virtual ~UpdateComponentCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    void           setInput(const Polyglot::Node& node) override;
    nlohmann::json execute(Database& database) override;

    bool   isUndoable() const override;
    void   undo(Database& database) override;
    void   redo(Database& database) override;
    size_t getUndoSize() const override;

  private:
    CADacomb::GUID assetGuid;
    CADacomb::GUID componentGuid;
    std::string    name;
    bool           hasName = false;
    nlohmann::json data;
    bool           hasData = false;
    nlohmann::json previousJson;
    nlohmann::json updatedJson;
};
