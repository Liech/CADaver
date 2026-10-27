#pragma once

#include "Command.h"

// Command to save an SVO voxel asset to an output stream.
class SaveVoxelAssetCommand : public Command
{
  public:
    SaveVoxelAssetCommand();
    explicit SaveVoxelAssetCommand(int assetIndex);
    explicit SaveVoxelAssetCommand(const nlohmann::json& json);
    virtual ~SaveVoxelAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    int  getAssetIndex() const;
    void setAssetIndex(int index);

  private:
    int assetIndex = -1;
};
