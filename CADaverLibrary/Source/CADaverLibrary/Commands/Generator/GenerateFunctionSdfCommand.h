#pragma once

#include "CADacomb/Formula/Formula.h"
#include "CADacomb/Util/AABB3.h"
#include "Execution/Command.h"

#include <memory>
#include <string>

class Asset;

// Command to generate a headless SdfAsset from a Slang-dialect Formula.
class GenerateFunctionSdfCommand : public Command
{
  public:
    GenerateFunctionSdfCommand();
    virtual ~GenerateFunctionSdfCommand();

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
    std::shared_ptr<Asset> createdAsset;
    CADacomb::Formula      formula;
    CADacomb::AABB3        bounds{ glm::dvec3(-1.0), glm::dvec3(1.0) };
    std::string            assetName = "FunctionSDF";
};
