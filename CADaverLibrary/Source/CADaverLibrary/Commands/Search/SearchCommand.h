#pragma once

#include "Execution/Command.h"

#include <string>

// Command to execute coarse, token- and fuzzy-based search across commands, concepts, and assets.
class SearchCommand : public Command
{
  public:
    SearchCommand();
    virtual ~SearchCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

  private:
    std::string query;
    std::string category;
    int         maxResults;
};
