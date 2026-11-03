#pragma once

#include "Windows/Window.h"

#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

class ConsoleStreamBuffer;

// UI window displaying console output, logs, and error messages.
class ConsoleWindow : public Window
{
  public:
    ConsoleWindow(bool autoAttach = true);
    virtual ~ConsoleWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void log(const std::string& message);
    void logError(const std::string& message);
    void clear();

    void attachStreams();
    void detachStreams();

    const std::vector<std::pair<std::string, bool>> getMessages() const;
    size_t                                          getMessageCount() const;

  private:
    void renderControls();
    void renderLogs();

    std::vector<std::pair<std::string, bool>> messages;
    bool                                      autoScroll = true;
    std::unique_ptr<ConsoleStreamBuffer>      coutBuffer;
    std::unique_ptr<ConsoleStreamBuffer>      cerrBuffer;
    mutable std::mutex                        messagesMutex;
};
