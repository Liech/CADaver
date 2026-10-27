#pragma once

#include "Commands/Command.h"

#include <string>

// Command to save a CAD asset to a STEP file.
class SaveStepAssetCommand : public Command
{
  public:
    SaveStepAssetCommand();
    explicit SaveStepAssetCommand(int assetIndex, const std::string& filePath = "");
    explicit SaveStepAssetCommand(const nlohmann::json& json);
    virtual ~SaveStepAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    int  getAssetIndex() const;
    void setAssetIndex(int index);

    const std::string& getFilePath() const;
    void               setFilePath(const std::string& newFilePath);

  private:
    std::string filePath;
    int         assetIndex = -1;
};
