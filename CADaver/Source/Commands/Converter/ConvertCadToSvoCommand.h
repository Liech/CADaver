#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Command to convert a CAD asset to a volumetric SVO asset via triangulation.
class ConvertCadToSvoCommand : public Command
{
  public:
    ConvertCadToSvoCommand();
    explicit ConvertCadToSvoCommand(const CADacomb::GUID& assetGuid, double precision = 0.1, uint32_t dimension = 64, bool deleteOrigin = false);
    explicit ConvertCadToSvoCommand(const nlohmann::json& json);
    virtual ~ConvertCadToSvoCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual std::vector<CADacomb::GUID> getRequiredAssets() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const CADacomb::GUID& getAssetGuid() const;
    void                  setAssetGuid(const CADacomb::GUID& newGuid);
    double                getPrecision() const;
    void                  setPrecision(double newPrecision);
    uint32_t              getDimension() const;
    void                  setDimension(uint32_t newDimension);
    bool                  getDeleteOrigin() const;
    void                  setDeleteOrigin(bool shouldDelete);

  private:
    CADacomb::GUID assetGuid;
    double         precision    = 0.1;
    uint32_t       dimension    = 64;
    bool           deleteOrigin = false;
};
