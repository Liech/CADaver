#pragma once

#include "CADacomb/Util/GUID.h"
#include "CADacomb/Volume/SVO/Generator/LandscapeConfig.h"
#include "Commands/Command.h"

#include <cstdint>
#include <string>

// Command to generate a volumetric landscape SVO asset.
class GenerateLandscapeCommand : public Command
{
  public:
    GenerateLandscapeCommand();
    explicit GenerateLandscapeCommand(const CADacomb::LandscapeConfig& config, uint32_t dimension = 64, const std::string& assetName = "Landscape");
    explicit GenerateLandscapeCommand(const nlohmann::json& json);
    virtual ~GenerateLandscapeCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const CADacomb::LandscapeConfig& getConfig() const;
    void                             setConfig(const CADacomb::LandscapeConfig& newConfig);

    uint32_t getDimension() const;
    void     setDimension(uint32_t newDimension);

    const std::string& getAssetName() const;
    void               setAssetName(const std::string& newName);

  private:
    CADacomb::LandscapeConfig config;
    uint32_t                  dimension = 64;
    std::string               assetName = "Landscape";
};
