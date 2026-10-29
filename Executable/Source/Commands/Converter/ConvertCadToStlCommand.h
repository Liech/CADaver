#pragma once

#include "CADacomb/Util/GUID.h"
#include "Commands/Command.h"

// Command to convert a CAD asset to a triangulation STL asset.
class ConvertCadToStlCommand : public Command
{
  public:
    ConvertCadToStlCommand();
    explicit ConvertCadToStlCommand(const CADacomb::GUID& assetGuid, double precision = 0.1, bool deleteOrigin = false);
    explicit ConvertCadToStlCommand(const nlohmann::json& json);
    virtual ~ConvertCadToStlCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual std::vector<CADacomb::GUID> getRequiredAssets() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const CADacomb::GUID& getAssetGuid() const;
    void                  setAssetGuid(const CADacomb::GUID& newGuid);
    double                getPrecision() const;
    void                  setPrecision(double newPrecision);
    bool                  getDeleteOrigin() const;
    void                  setDeleteOrigin(bool shouldDelete);

  private:
    CADacomb::GUID assetGuid;
    double         precision    = 0.1;
    bool           deleteOrigin = false;
};
