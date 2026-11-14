#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>
#include <nlohmann/json.hpp>
#include <string>

class AssetComponent;

// Generic command to create and attach any registered AssetComponent to an asset.
class AddComponentCommand : public Command
{
  public:
    AddComponentCommand();
    virtual ~AddComponentCommand();

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
    CADacomb::GUID                  assetGuid;
    std::string                     componentType;
    std::string                     name;
    nlohmann::json                  data;
    bool                            hasData = false;
    std::shared_ptr<AssetComponent> addedComponent;
};
