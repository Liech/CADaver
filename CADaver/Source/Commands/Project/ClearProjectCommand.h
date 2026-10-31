#pragma once

#include "Execution/Command.h"

#include <memory>
#include <vector>

class Asset;

// Command to clear all assets from the project.
class ClearProjectCommand : public Command
{
  public:
    ClearProjectCommand();
    explicit ClearProjectCommand(const nlohmann::json& json);
    virtual ~ClearProjectCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    std::vector<std::shared_ptr<Asset>> clearedAssets;
};
