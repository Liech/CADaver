#include "GatedDatabase.h"

#include <cassert>
#include <iostream>

GatedDatabase::GatedDatabase() {}

GatedDatabase::~GatedDatabase() {}

GatedDatabase& GatedDatabase::getInstance()
{
    static GatedDatabase instance;
    return instance;
}

const Database& GatedDatabase::get() const
{
    return database;
}

nlohmann::json GatedDatabase::execute(const nlohmann::json& command, std::istream* inputStream, std::ostream* outputStream)
{
    std::string commandType;
    int         commandVersion = 0;
    std::cout << "IN: " << std::endl << command.dump(4) << std::endl;
    try
    {
        if (command.contains("type"))
        {
            commandType = command.at("type").get<std::string>();
        }
        else
        {
            commandType = command.at("command").get<std::string>();
        }
        commandVersion = command.at("version").get<int>();

        auto cmd = commandFactory.create(command);
        if (!cmd)
        {
            nlohmann::json result = {
                {    "type",                                "Error" },
                { "command",                            commandType },
                { "version",                         commandVersion },
                {  "status",                       "UnknownCommand" },
                {   "error", "Unknown command type: " + commandType }
            };
            std::cout << "OUT: " << std::endl << result.dump(4) << std::endl;
            return result;
        }

        nlohmann::json result = cmd->execute(database, inputStream, outputStream);
        assert(result.at("type").get<std::string>() == cmd->getType());
        std::cout << "OUT: " << std::endl << result.dump(4) << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        nlohmann::json result = {
            {    "type",        "Error" },
            { "command",    commandType },
            { "version", commandVersion },
            {  "status",    "Exception" },
            {   "error",       e.what() }
        };
        std::cout << "OUT: " << std::endl << result.dump(4) << std::endl;
        return result;
    }
}

std::string GatedDatabase::execute(const std::string& commandString, std::istream* inputStream, std::ostream* outputStream)
{
    nlohmann::json parsedCommand;
    try
    {
        parsedCommand = nlohmann::json::parse(commandString);
    }
    catch (const nlohmann::json::parse_error& e)
    {
        nlohmann::json errorResult = {
            {    "type",                                      "Error" },
            { "command",                                           "" },
            { "version",                                            0 },
            {  "status",                             "JsonParseError" },
            {   "error", std::string("JSON parse error: ") + e.what() }
        };
        return errorResult.dump();
    }

    nlohmann::json result = execute(parsedCommand, inputStream, outputStream);
    return result.dump();
}
