#pragma once

#include "Execution/Command.h"

#include <cstddef>
#include <memory>
#include <string>

class Asset;

// Command to generate a parametric base triangulation shape asset.
class GenerateBaseShapeCommand : public Command
{
  public:
    GenerateBaseShapeCommand();
    virtual ~GenerateBaseShapeCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    std::string            shape         = "cube";
    std::string            assetName     = "";
    float                  width         = 1.0f;
    float                  depth         = 1.0f;
    float                  height        = 1.0f;
    float                  length        = 2.0f;
    float                  thickness     = 0.3f;
    float                  radius        = 0.5f;
    float                  bottomRadius  = 1.0f;
    float                  topRadius     = 0.5f;
    float                  majorRadius   = 1.0f;
    float                  minorRadius   = 0.25f;
    float                  holeRadius    = 0.3f;
    size_t                 segments      = 32;
    size_t                 rings         = 16;
    size_t                 majorSegments = 32;
    size_t                 minorSegments = 16;
    size_t                 sides         = 6;
    std::shared_ptr<Asset> createdAsset;
};
