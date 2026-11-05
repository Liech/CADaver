#pragma once

#include "Execution/Command.h"

#include <string>

// Command to retrieve full documentation, specification, or full text of commands, concepts, assets, or notes.
class GetDocumentationCommand : public Command
{
  public:
    GetDocumentationCommand();
    virtual ~GetDocumentationCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    std::string category;
    std::string name;
};
