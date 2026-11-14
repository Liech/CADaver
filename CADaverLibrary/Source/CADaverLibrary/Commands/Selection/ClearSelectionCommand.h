#pragma once

#include "Execution/Command.h"

// Command to clear all current selections in the database.
class ClearSelectionCommand : public Command
{
  public:
    ClearSelectionCommand();
    virtual ~ClearSelectionCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;
};
