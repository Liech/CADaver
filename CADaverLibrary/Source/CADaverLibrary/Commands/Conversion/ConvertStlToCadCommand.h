#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>
#include <string>
#include <vector>

class CadAsset;

// Converts a TriangulationAsset naively (one planar CAD face per triangle) into a CadAsset.
class ConvertStlToCadCommand : public Command
{
  public:
    ConvertStlToCadCommand();
    virtual ~ConvertStlToCadCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    std::vector<CADacomb::GUID> getRequiredAssets() const override;

    void           setInput(const Polyglot::Node& node) override;
    nlohmann::json execute(Database& database) override;

    bool   isUndoable() const override;
    void   undo(Database& database) override;
    void   redo(Database& database) override;
    size_t getUndoSize() const override;

  private:
    CADacomb::GUID            assetGuid;
    double                    stitchTolerance = 1e-4;
    bool                      tryMakeSolid    = true;
    std::shared_ptr<CadAsset> createdAsset;
};
