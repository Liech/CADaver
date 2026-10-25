#pragma once

#include "Command.h"

// Command to save a triangulation asset to a binary STL output stream.
class SaveStlAssetCommand : public Command
{
  public:
    SaveStlAssetCommand();
    explicit SaveStlAssetCommand(int assetIndex);
    explicit SaveStlAssetCommand(const nlohmann::json& json);
    virtual ~SaveStlAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream) override;

    int  getAssetIndex() const;
    void setAssetIndex(int index);

  private:
    int assetIndex = -1;
};
