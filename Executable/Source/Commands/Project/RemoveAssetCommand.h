#pragma once

#include "Commands/Command.h"

// Command to remove an asset from the project by index.
class RemoveAssetCommand : public Command
{
  public:
    RemoveAssetCommand();
    explicit RemoveAssetCommand(int assetIndex);
    explicit RemoveAssetCommand(const nlohmann::json& json);
    virtual ~RemoveAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    int  getAssetIndex() const;
    void setAssetIndex(int index);

  private:
    int assetIndex = -1;
};
