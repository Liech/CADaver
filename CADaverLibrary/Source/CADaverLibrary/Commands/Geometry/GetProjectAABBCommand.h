#pragma once

#include "Execution/Command.h"

// Command to query the combined axis-aligned bounding box of all geometry assets in the project.
class GetProjectAABBCommand : public Command
{
  public:
    GetProjectAABBCommand();
    virtual ~GetProjectAABBCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database) override;
};
