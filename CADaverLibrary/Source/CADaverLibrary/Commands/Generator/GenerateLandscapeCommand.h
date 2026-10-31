#pragma once

#include "CADacomb/Util/GUID.h"
#include "CADacomb/Volume/SVO/Generator/LandscapeConfig.h"
#include "Execution/Command.h"

#include <cstdint>
#include <memory>
#include <string>

class Asset;

// Command to generate a volumetric landscape SVO asset.
class GenerateLandscapeCommand : public Command
{
  public:
    GenerateLandscapeCommand();
    virtual ~GenerateLandscapeCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    CADacomb::LandscapeConfig config;
    std::shared_ptr<Asset>    createdAsset;
    uint32_t                  dimension = 64;
    std::string               assetName = "Landscape";
};
