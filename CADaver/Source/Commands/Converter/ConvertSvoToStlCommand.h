#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>

class Asset;

// Command to convert a volumetric SVO asset to a blocky triangulation STL asset.
class ConvertSvoToStlCommand : public Command
{
  public:
    ConvertSvoToStlCommand();
    explicit ConvertSvoToStlCommand(const CADacomb::GUID& assetGuid, int targetDepth = -1, bool deleteOrigin = false);
    explicit ConvertSvoToStlCommand(const nlohmann::json& json);
    virtual ~ConvertSvoToStlCommand();

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

    const CADacomb::GUID& getAssetGuid() const;
    void                  setAssetGuid(const CADacomb::GUID& newGuid);
    int                   getTargetDepth() const;
    void                  setTargetDepth(int newDepth);
    bool                  getDeleteOrigin() const;
    void                  setDeleteOrigin(bool shouldDelete);

  private:
    CADacomb::GUID         assetGuid;
    std::shared_ptr<Asset> originAsset;
    std::shared_ptr<Asset> createdAsset;
    int                    targetDepth  = -1;
    bool                   deleteOrigin = false;
};
