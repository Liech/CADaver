#include "MCP/InteractiveMcpRunner.h"

#include "Cadaver.h"
#include "CommandLineArgs.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "MCP/McpServer.h"
#include "Polyglot/Node.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantWindow.h"
#include "Vulcant/VulcantV/VulcantVDevice.h"
#include "Vulcant/Wrapper/Window.h"
#include "Windows/Console/SystemConsole.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

InteractiveMcpRunner::InteractiveMcpRunner() {}

InteractiveMcpRunner::~InteractiveMcpRunner()
{
    stopMcpListener();
}

int InteractiveMcpRunner::run(const CommandLineArgs& args)
{
    SystemConsole::setAvailable(false);

    GatedDatabase startupDatabase;
    if (args.isVerbose())
    {
        startupDatabase.setLogCallback([](const std::string& msg) { std::cerr << msg << std::endl; });
    }
    else
    {
        startupDatabase.setLogCallback([](const std::string&) {});
    }

    McpServer mcpServer(startupDatabase);

    std::atomic<Cadaver*> activeApp{ nullptr };
    std::atomic<bool>     startupInProgress{ true };

    mcpServer.setCommandExecutor(
      [&activeApp, &startupInProgress, &startupDatabase](const nlohmann::json& command) -> nlohmann::json
      {
          while (startupInProgress.load() && activeApp.load() == nullptr)
          {
              std::this_thread::sleep_for(std::chrono::milliseconds(10));
          }

          if (activeApp.load() != nullptr)
          {
              Cadaver* app     = activeApp.load();
              auto     promise = std::make_shared<std::promise<nlohmann::json>>();
              auto     future  = promise->get_future();

              app->getDefer().defer(
                [app, command, promise]()
                {
                    Polyglot::Node input(command);
                    Polyglot::Node result = app->getGatedDatabase().execute(input);
                    promise->set_value(result.toJson());
                });

              return future.get();
          }

          Polyglot::Node input(command);
          Polyglot::Node result = startupDatabase.execute(input);
          return result.toJson();
      });

    startMcpListener(mcpServer);

    try
    {
        auto                              windowExtensions = Vulcant::Wrapper::Window::getVulkanExtensions();
        Vulcant::VulcantV::VulcantVDevice device(windowExtensions);
        {
            Cadaver app(device, glm::ivec2(1280, 720));

            if (args.isVerbose())
            {
                app.getGatedDatabase().setLogCallback([](const std::string& msg) { std::cerr << msg << std::endl; });
            }
            else
            {
                app.getGatedDatabase().setLogCallback([](const std::string&) {});
            }

            activeApp.store(&app);
            startupInProgress.store(false);

            auto& window = app.getWindow();
            while (!window.isClosed() && !app.isQuitRequested())
            {
                window.tick();
            }

            activeApp.store(nullptr);
            stopMcpListener();
        }
    }
    catch (const std::exception& e)
    {
        startupInProgress.store(false);
        std::cerr << "Interactive MCP error: " << e.what() << std::endl;
        stopMcpListener();
        return EXIT_FAILURE;
    }
    catch (...)
    {
        startupInProgress.store(false);
        std::cerr << "Interactive MCP unknown error" << std::endl;
        stopMcpListener();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void InteractiveMcpRunner::startMcpListener(McpServer& mcpServer)
{
    running   = true;
    mcpThread = std::make_unique<std::thread>(
      [this, &mcpServer]()
      {
          std::string line;
          while (running && pipeStream.readLine(line))
          {
              if (line.empty())
              {
                  continue;
              }

              std::string response = mcpServer.handleMessage(line);
              if (!response.empty())
              {
                  std::lock_guard<std::mutex> lock(outputMutex);
                  pipeStream.writeLine(response);
              }
          }
      });
}

void InteractiveMcpRunner::stopMcpListener()
{
    running = false;
    if (mcpThread && mcpThread->joinable())
    {
        mcpThread->detach();
    }
}
