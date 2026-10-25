#pragma once

#include "Command.h"

// Command to change the selected asset index in the project.
class SelectAssetCommand : public Command
{
  public:
    SelectAssetCommand();
    explicit SelectAssetCommand(int index);
    explicit SelectAssetCommand(const nlohmann::json& json);
    virtual ~SelectAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream) override;

    int  getIndex() const;
    void setIndex(int newIndex);

  private:
    int index = -1;
};
