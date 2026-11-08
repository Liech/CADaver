#pragma once

#include "Windows/Window.h"

#include <string>

// Dialog window displaying task completion, cancellation, or error notifications.
class TaskNotificationModal : public Window
{
  public:
    TaskNotificationModal();
    virtual ~TaskNotificationModal();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    const std::string& getMessage() const;
    void               setMessage(const std::string& messageInput);

  private:
    std::string message;
};
