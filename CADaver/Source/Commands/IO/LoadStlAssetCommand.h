#pragma once

#include "Commands/Command.h"

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

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const std::string& getName() const;
    void               setName(const std::string& newName);

  private:
    std::string name;
};
