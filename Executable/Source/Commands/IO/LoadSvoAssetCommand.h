#pragma once

#include "Commands/Command.h"

#include <string>

// Command to load an SVO voxel asset from an input stream or file path into the project.
class LoadSvoAssetCommand : public Command
{
  public:
    LoadSvoAssetCommand();
    explicit LoadSvoAssetCommand(const std::string& name, const std::string& filePath = "");
    explicit LoadSvoAssetCommand(const nlohmann::json& json);
    virtual ~LoadSvoAssetCommand();

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
