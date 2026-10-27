#pragma once

#include "Command.h"

// Command to update render preferences (mesh color, wireframe mode, wireframe color, cull mode).
class UpdateRenderPreferencesCommand : public Command
{
  public:
    UpdateRenderPreferencesCommand();
    explicit UpdateRenderPreferencesCommand(const nlohmann::json& json);
    virtual ~UpdateRenderPreferencesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const nlohmann::json& getPreferencesData() const;
    void                  setPreferencesData(const nlohmann::json& data);

  private:
    nlohmann::json preferencesData;
};
