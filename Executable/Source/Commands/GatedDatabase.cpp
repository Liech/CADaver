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
    if (locked)
    {
        nlohmann::json result = {
            {    "type",                                      "Error" },
            { "command",                                           "" },
            { "version",                                            0 },
            {  "status",                                     "Locked" },
            {   "error", "Only one command can be executed at a time" }
        };
        log("OUT: " + result.dump(4));
        return result;
    }

    std::string commandType;
    int         commandVersion = 0;
    log("IN: " + command.dump(4));
    try
    {
        commandType    = command.at("type").get<std::string>();
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
            log("OUT: " + result.dump(4));
            return result;
        }

        locked                = true;
        nlohmann::json result = cmd->execute(database, inputStream, outputStream);
        locked                = false;
        assert(result.at("type").get<std::string>() == cmd->getType());
        log("OUT: " + result.dump(4));
        return result;
    }
    catch (const std::exception& e)
    {
        locked                = false;
        nlohmann::json result = {
            {    "type",        "Error" },
            { "command",    commandType },
            { "version", commandVersion },
            {  "status",    "Exception" },
            {   "error",       e.what() }
        };
        log("OUT: " + result.dump(4));
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

void GatedDatabase::log(const std::string& msg)
{
    std::cout << msg << std::endl;
}
