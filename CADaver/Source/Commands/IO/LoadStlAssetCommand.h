#pragma once

#include "Execution/Command.h"

#include <string>

// Command to load an STL triangulation asset from an input stream into the project.
class LoadStlAssetCommand : public Command
{
  public:
    LoadStlAssetCommand();
    explicit LoadStlAssetCommand(const std::string& name);
    explicit LoadStlAssetCommand(const nlohmann::json& json);
    virtual ~LoadStlAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    const std::string& getName() const;
    void               setName(const std::string& newName);

  private:
    std::string name;
};
