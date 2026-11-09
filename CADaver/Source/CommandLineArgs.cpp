#include "CommandLineArgs.h"

#include <vector>

CommandLineArgs::CommandLineArgs() {}

CommandLineArgs::~CommandLineArgs() {}

CommandLineArgs CommandLineArgs::parse(int argc, char* argv[])
{
    CommandLineArgs result;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if ((arg == "--script" || arg == "-s") && i + 1 < argc)
        {
            i++;
            result.setScriptPath(argv[i]);
        }
        else if ((arg == "--lang" || arg == "-l") && i + 1 < argc)
        {
            i++;
            result.setLanguage(argv[i]);
        }
        else if ((arg == "--output" || arg == "-o") && i + 1 < argc)
        {
            i++;
            result.setOutputPath(argv[i]);
        }
        else if (arg == "--verbose" || arg == "-v")
        {
            result.setVerbose(true);
        }
        else if (arg == "--mcp")
        {
            result.setMcp(true);
        }
        else if (arg == "--headless")
        {
            result.setHeadless(true);
        }
        else if ((arg == "--port" || arg == "-p") && i + 1 < argc)
        {
            i++;
            result.setPort(std::stoi(argv[i]));
            result.setMcp(true);
        }
    }

    return result;
}

bool CommandLineArgs::hasScript() const
{
    return !scriptPath.empty();
}

bool CommandLineArgs::hasMcp() const
{
    return mcp;
}

bool CommandLineArgs::isHeadless() const
{
    return headless;
}

const std::string& CommandLineArgs::getScriptPath() const
{
    return scriptPath;
}

const std::string& CommandLineArgs::getLanguage() const
{
    return language;
}

bool CommandLineArgs::isVerbose() const
{
    return verbose;
}

const std::string& CommandLineArgs::getOutputPath() const
{
    return outputPath;
}

int CommandLineArgs::getPort() const
{
    return port;
}

void CommandLineArgs::setScriptPath(const std::string& path)
{
    scriptPath = path;
}

void CommandLineArgs::setLanguage(const std::string& lang)
{
    language = lang;
}

void CommandLineArgs::setVerbose(bool flag)
{
    verbose = flag;
}

void CommandLineArgs::setOutputPath(const std::string& path)
{
    outputPath = path;
}

void CommandLineArgs::setMcp(bool flag)
{
    mcp = flag;
}

void CommandLineArgs::setHeadless(bool flag)
{
    headless = flag;
}

void CommandLineArgs::setPort(int portNumber)
{
    port = portNumber;
}
