#pragma once

#include "Execution/Command.h"

class UIElementFactory;

// Command to trigger clicking on a registered UIElement or MenuEntry by its type, path, or label.
class ClickUIElementCommand : public Command
{
  public:
    explicit ClickUIElementCommand(UIElementFactory& factory);
    virtual ~ClickUIElementCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

    const std::string& getTarget() const;

  private:
    UIElementFactory& factory;
    std::string       target;
};
