#pragma once

#include "Execution/Command.h"

#include <string>

// Command to load an SVO voxel asset from an input stream into the project.
class LoadSvoAssetCommand : public Command
{
  public:
    LoadSvoAssetCommand();
    explicit LoadSvoAssetCommand(const std::string& name);
    explicit LoadSvoAssetCommand(const nlohmann::json& json);
    virtual ~LoadSvoAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const std::string& getName() const;
    void               setName(const std::string& newName);

  private:
    std::string name;
};
