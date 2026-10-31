#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <string>
#include <vector>

// Command to save a CAD asset to a STEP file.
class SaveStepAssetCommand : public Command
{
  public:
    SaveStepAssetCommand();
    explicit SaveStepAssetCommand(const CADacomb::GUID& assetGuid, const std::string& filePath = "");
    explicit SaveStepAssetCommand(const nlohmann::json& json);
    virtual ~SaveStepAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual std::vector<CADacomb::GUID> getRequiredAssets() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const CADacomb::GUID& getAssetGuid() const;
    void                  setAssetGuid(const CADacomb::GUID& guid);

    const std::string& getFilePath() const;
    void               setFilePath(const std::string& newFilePath);

  private:
    std::string    filePath;
    CADacomb::GUID assetGuid;
};
