#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Command to convert a triangulation STL asset to a volumetric SVO asset.
class ConvertStlToSvoCommand : public Command
{
  public:
    ConvertStlToSvoCommand();
    explicit ConvertStlToSvoCommand(const CADacomb::GUID& assetGuid, uint32_t dimension = 64, bool deleteOrigin = false);
    explicit ConvertStlToSvoCommand(const nlohmann::json& json);
    virtual ~ConvertStlToSvoCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual std::vector<CADacomb::GUID> getRequiredAssets() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const CADacomb::GUID& getAssetGuid() const;
    void                  setAssetGuid(const CADacomb::GUID& newGuid);
    uint32_t              getDimension() const;
    void                  setDimension(uint32_t newDimension);
    bool                  getDeleteOrigin() const;
    void                  setDeleteOrigin(bool shouldDelete);

  private:
    CADacomb::GUID assetGuid;
    uint32_t       dimension    = 64;
    bool           deleteOrigin = false;
};
