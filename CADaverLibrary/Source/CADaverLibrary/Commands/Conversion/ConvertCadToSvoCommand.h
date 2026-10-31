#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>

class Asset;

// Command to convert a CAD asset to a volumetric SVO asset via triangulation.
class ConvertCadToSvoCommand : public Command
{
  public:
    ConvertCadToSvoCommand();
    virtual ~ConvertCadToSvoCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual std::vector<CADacomb::GUID> getRequiredAssets() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    CADacomb::GUID         assetGuid;
    std::shared_ptr<Asset> originAsset;
    std::shared_ptr<Asset> createdAsset;
    double                 precision    = 0.1;
    uint32_t               dimension    = 64;
    bool                   deleteOrigin = false;
};
