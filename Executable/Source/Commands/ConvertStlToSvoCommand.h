#pragma once

#include "Command.h"

// Command to convert a triangulation STL asset to a volumetric SVO asset.
class ConvertStlToSvoCommand : public Command
{
  public:
    ConvertStlToSvoCommand();
    explicit ConvertStlToSvoCommand(int assetIndex, uint32_t dimension = 64);
    explicit ConvertStlToSvoCommand(const nlohmann::json& json);
    virtual ~ConvertStlToSvoCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream) override;

    int      getAssetIndex() const;
    void     setAssetIndex(int newIndex);
    uint32_t getDimension() const;
    void     setDimension(uint32_t newDimension);

  private:
    int      assetIndex = -1;
    uint32_t dimension  = 64;
};
