#pragma once

#include "CADacomb/Util/GUID.h"
#include "Commands/Command.h"

// Command to save an SVO voxel asset to an output stream.
class SaveVoxelAssetCommand : public Command
{
  public:
    SaveVoxelAssetCommand();
    explicit SaveVoxelAssetCommand(const CADacomb::GUID& assetGuid);
    explicit SaveVoxelAssetCommand(const nlohmann::json& json);
    virtual ~SaveVoxelAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const CADacomb::GUID& getAssetGuid() const;
    void                  setAssetGuid(const CADacomb::GUID& guid);

  private:
    CADacomb::GUID assetGuid;
};
