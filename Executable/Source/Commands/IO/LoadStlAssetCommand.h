#pragma once

#include "Commands/Command.h"

#include <string>

// Command to load an STL triangulation asset from an input stream or file path into the project.
class LoadStlAssetCommand : public Command
{
  public:
    LoadStlAssetCommand();
    explicit LoadStlAssetCommand(const std::string& name, const std::string& filePath = "");
    explicit LoadStlAssetCommand(const nlohmann::json& json);
    virtual ~LoadStlAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const std::string& getName() const;
    void               setName(const std::string& newName);

    const std::string& getFilePath() const;
    void               setFilePath(const std::string& newFilePath);

  private:
    std::string name;
    std::string filePath;
};
