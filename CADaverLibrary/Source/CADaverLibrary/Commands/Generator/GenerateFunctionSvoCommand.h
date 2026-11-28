#pragma once

#include "CADacomb/Formula/Formula.h"
#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <cstdint>
#include <memory>
#include <string>

class Asset;

// Command to generate a volumetric SVO asset from a Slang-dialect Formula.
class GenerateFunctionSvoCommand : public Command
{
  public:
    GenerateFunctionSvoCommand();
    virtual ~GenerateFunctionSvoCommand();

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
    std::string            assetName = "FunctionSVO";
    uint32_t               dimension = 64;
};
