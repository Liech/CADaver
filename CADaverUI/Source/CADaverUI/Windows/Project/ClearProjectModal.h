#pragma once

#include "Windows/Window.h"

#include <string>

// Dialog window for confirming removal of all assets from the project.
class ClearProjectModal : public Window
{
  public:
    ClearProjectModal();
    virtual ~ClearProjectModal();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

  private:
    void executeClear();
};
