#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <glm/glm.hpp>
#include <map>
#include <vector>

// Command to step the project time forward, updating time-dependent systems with undo and command merging support.
class StepTimeCommand : public Command
{
  public:
    StepTimeCommand();
    virtual ~StepTimeCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    void           setInput(const Polyglot::Node& node) override;
    nlohmann::json execute(Database& database) override;

    bool   isUndoable() const override;
    void   undo(Database& database) override;
    void   redo(Database& database) override;
    size_t getUndoSize() const override;

    bool isMergeable() const override;
    bool merge(const Command& other) override;

    double getDeltaTime() const;

  private:
    double                                            deltaTime    = 1.0 / 60.0;
    double                                            previousTime = 0.0;
    double                                            currentTime  = 0.0;
    std::map<CADacomb::GUID, glm::dmat4>              previousTransformations;
    std::map<CADacomb::GUID, glm::dmat4>              currentTransformations;
    std::map<CADacomb::GUID, std::vector<glm::dvec3>> previousVertices;
    std::map<CADacomb::GUID, std::vector<glm::dvec3>> currentVertices;
};
