#pragma once

#include "Command.h"

// Command to convert a CAD asset to a triangulation STL asset.
class ConvertCadToStlCommand : public Command
{
  public:
    ConvertCadToStlCommand();
    explicit ConvertCadToStlCommand(int assetIndex, double precision = 0.1, bool deleteOrigin = false);
    explicit ConvertCadToStlCommand(const nlohmann::json& json);
    virtual ~ConvertCadToStlCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    int    getAssetIndex() const;
    void   setAssetIndex(int newIndex);
    double getPrecision() const;
    void   setPrecision(double newPrecision);
    bool   getDeleteOrigin() const;
    void   setDeleteOrigin(bool shouldDelete);

  private:
    int    assetIndex   = -1;
    double precision    = 0.1;
    bool   deleteOrigin = false;
};
