#pragma once

#include "Execution/Command.h"

// Command to update SVO LOD visualization preferences.
class UpdateSVOPreferencesCommand : public Command
{
  public:
    UpdateSVOPreferencesCommand();
    explicit UpdateSVOPreferencesCommand(const nlohmann::json& json);
    virtual ~UpdateSVOPreferencesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const nlohmann::json& getPreferencesData() const;
    void                  setPreferencesData(const nlohmann::json& data);

  private:
    nlohmann::json preferencesData;
};
