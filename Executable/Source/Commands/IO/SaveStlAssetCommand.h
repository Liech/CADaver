#pragma once

#include "CADacomb/Util/GUID.h"
#include "Commands/Command.h"

// Command to save a triangulation asset to a binary STL output stream.
class SaveStlAssetCommand : public Command
{
  public:
    SaveStlAssetCommand();
    explicit SaveStlAssetCommand(const CADacomb::GUID& assetGuid);
    explicit SaveStlAssetCommand(const nlohmann::json& json);
    virtual ~SaveStlAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const CADacomb::GUID& getAssetGuid() const;
    void                  setAssetGuid(const CADacomb::GUID& guid);

  private:
    CADacomb::GUID assetGuid;
};
