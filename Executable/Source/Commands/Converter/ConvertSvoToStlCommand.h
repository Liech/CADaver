#pragma once

#include "CADacomb/Util/GUID.h"
#include "Commands/Command.h"

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

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const CADacomb::GUID& getAssetGuid() const;
    void                  setAssetGuid(const CADacomb::GUID& newGuid);
    int                   getTargetDepth() const;
    void                  setTargetDepth(int newDepth);
    bool                  getDeleteOrigin() const;
    void                  setDeleteOrigin(bool shouldDelete);

  private:
    CADacomb::GUID assetGuid;
    int            targetDepth  = -1;
    bool           deleteOrigin = false;
};
