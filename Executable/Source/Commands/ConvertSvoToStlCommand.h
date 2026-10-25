#pragma once

#include "Command.h"

// Command to convert a volumetric SVO asset to a blocky triangulation STL asset.
class ConvertSvoToStlCommand : public Command
{
  public:
    ConvertSvoToStlCommand();
    explicit ConvertSvoToStlCommand(int assetIndex);
    explicit ConvertSvoToStlCommand(const nlohmann::json& json);
    virtual ~ConvertSvoToStlCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream) override;

    int  getAssetIndex() const;
    void setAssetIndex(int newIndex);

  private:
    int assetIndex = -1;
};
