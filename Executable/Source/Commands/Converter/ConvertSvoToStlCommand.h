#pragma once

#include "Commands/Command.h"

// Command to convert a volumetric SVO asset to a blocky triangulation STL asset.
class ConvertSvoToStlCommand : public Command
{
  public:
    ConvertSvoToStlCommand();
    explicit ConvertSvoToStlCommand(int assetIndex, int targetDepth = -1, bool deleteOrigin = false);
    explicit ConvertSvoToStlCommand(const nlohmann::json& json);
    virtual ~ConvertSvoToStlCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    int  getAssetIndex() const;
    void setAssetIndex(int newIndex);
    int  getTargetDepth() const;
    void setTargetDepth(int newDepth);
    bool getDeleteOrigin() const;
    void setDeleteOrigin(bool shouldDelete);

  private:
    int  assetIndex   = -1;
    int  targetDepth  = -1;
    bool deleteOrigin = false;
};
