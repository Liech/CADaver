#pragma once

#include "CADacomb/Util/GUID.h"
#include "Database/Selection/SelectionEntry.h"
#include "Execution/Command.h"

#include <memory>
#include <string>
#include <vector>

class FaceValuesComponent;

// Clusters triangulation faces by normal angle threshold and attaches a FaceValuesComponent.
class ClusterByNormalCommand : public Command
{
  public:
    ClusterByNormalCommand();
    virtual ~ClusterByNormalCommand();

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
    CADacomb::GUID                       assetGuid;
    double                               maxAngleDegrees = 35.0;
    bool                                 withoutHoles    = false;
    std::string                          componentName   = "Normal Clusters";
    std::shared_ptr<FaceValuesComponent> addedComponent;
    CADacomb::GUID                       addedComponentGuid;
    std::vector<SelectionEntry>          previousSelection;
};
