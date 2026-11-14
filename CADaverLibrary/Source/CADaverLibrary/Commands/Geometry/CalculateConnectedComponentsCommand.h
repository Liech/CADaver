#pragma once

#include "CADacomb/Util/GUID.h"
#include "Database/Selection/Selection.h"
#include "Execution/Command.h"

#include <memory>
#include <string>

class FaceValuesComponent;

// Calculates connected components on a TriangulationAsset and attaches a FaceValuesComponent.
class CalculateConnectedComponentsCommand : public Command
{
  public:
    CalculateConnectedComponentsCommand();
    virtual ~CalculateConnectedComponentsCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    void           setInput(const Polyglot::Node& node) override;
    nlohmann::json execute(Database& database) override;

    bool   isUndoable() const override;
    void   undo(Database& database) override;
    void   redo(Database& database) override;
    size_t getUndoSize() const override;

  private:
    CADacomb::GUID                       assetGuid;
    std::string                          name;
    std::shared_ptr<FaceValuesComponent> addedComponent;
    Selection                            previousSelection;
};
