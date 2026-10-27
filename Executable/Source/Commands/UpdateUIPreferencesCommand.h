#pragma once

#include "Command.h"

// Command to update UI preferences (theme, UI scale, background color).
class UpdateUIPreferencesCommand : public Command
{
  public:
    UpdateUIPreferencesCommand();
    explicit UpdateUIPreferencesCommand(const nlohmann::json& json);
    virtual ~UpdateUIPreferencesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const nlohmann::json& getPreferencesData() const;
    void                  setPreferencesData(const nlohmann::json& data);

  private:
    nlohmann::json preferencesData;
};
