#pragma once

#include "Execution/Command.h"

#include <memory>
#include <string>

class Asset;

// Command to load a STEP CAD asset into the project.
class LoadStepAssetCommand : public Command
{
  public:
    LoadStepAssetCommand();
    LoadStepAssetCommand(const std::string& name, const std::string& filePath);
    explicit LoadStepAssetCommand(const nlohmann::json& json);
    virtual ~LoadStepAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

    const std::string& getName() const;
    void               setName(const std::string& newName);

    const std::string& getFilePath() const;
    void               setFilePath(const std::string& newFilePath);

  private:
    std::string            name;
    std::string            filePath;
    std::shared_ptr<Asset> createdAsset;
};
