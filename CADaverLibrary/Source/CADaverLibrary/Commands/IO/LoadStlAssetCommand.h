#pragma once

#include "Execution/Command.h"

#include <memory>
#include <string>

class Asset;

// Command to load an STL triangulation asset from an input stream into the project.
class LoadStlAssetCommand : public Command
{
  public:
    LoadStlAssetCommand();
    virtual ~LoadStlAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    std::string            name;
    std::shared_ptr<Asset> createdAsset;
};
