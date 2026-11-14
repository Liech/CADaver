#pragma once

#include "Execution/Command.h"

#include <glm/glm.hpp>

// Command to perform CPU raycasting against project assets along a world-space ray.
class PickAssetCommand : public Command
{
  public:
    PickAssetCommand();
    virtual ~PickAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    glm::dvec3 origin    = glm::dvec3(0.0);
    glm::dvec3 direction = glm::dvec3(0.0, 0.0, -1.0);
};
