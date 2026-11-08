#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>
#include <string>

class AssetComponent;

// Command to remove any component by GUID from its parent asset indiscriminately.
class RemoveComponentCommand : public Command
{
  public:
    RemoveComponentCommand();
    virtual ~RemoveComponentCommand();

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
    CADacomb::GUID                  componentGuid;
    CADacomb::GUID                  assetGuid;
    std::shared_ptr<AssetComponent> removedComponent;
};
