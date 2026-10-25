#pragma once

#include "Command.h"

#include <string>

// Command to load a STEP CAD asset from an input stream into the project.
class LoadStepAssetCommand : public Command
{
  public:
    LoadStepAssetCommand();
    explicit LoadStepAssetCommand(const std::string& name);
    explicit LoadStepAssetCommand(const nlohmann::json& json);
    virtual ~LoadStepAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream) override;

    const std::string& getName() const;
    void               setName(const std::string& newName);

  private:
    std::string name;
};
