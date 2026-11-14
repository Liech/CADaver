#pragma once

#include "Database/AssetComponent.h"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

// Assigns indexed JSON values to triangulation faces with a Lua color mapping script.
class FaceValuesComponent : public AssetComponent
{
  public:
    FaceValuesComponent();
    virtual ~FaceValuesComponent();

    std::string getType() const override;
    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;
    bool        isCompatible(const std::string& assetType) const override;

    nlohmann::json toDataJson() const override;
    void           fromDataJson(const nlohmann::json& jsonObject) override;

    const std::vector<nlohmann::json>& getValues() const;
    void                               setValues(const std::vector<nlohmann::json>& values);

    const std::vector<uint32_t>& getFaceValueIndices() const;
    void                         setFaceValueIndices(const std::vector<uint32_t>& faceValueIndices);

    const nlohmann::json& getMetadata() const;
    void                  setMetadata(const nlohmann::json& metadata);

    const std::string& getLuaScript() const;
    void               setLuaScript(const std::string& luaScript);

    static std::string getDefaultLuaScript();

  private:
    std::vector<nlohmann::json> values;
    std::vector<uint32_t>       faceValueIndices;
    nlohmann::json              metadata;
    std::string                 luaScript;
};
