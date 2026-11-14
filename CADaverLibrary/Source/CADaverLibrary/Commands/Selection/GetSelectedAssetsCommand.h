#pragma once

#include "Execution/Command.h"

// Command to query the list of currently selected asset GUIDs from the database.
class GetSelectedAssetsCommand : public Command
{
  public:
    GetSelectedAssetsCommand();
    virtual ~GetSelectedAssetsCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;
};
