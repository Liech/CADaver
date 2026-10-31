#pragma once

#include "Execution/Command.h"

// Command to update camera preferences (FOV, clip planes, resolution, sensitivity, mode).
class UpdateCameraPreferencesCommand : public Command
{
  public:
    UpdateCameraPreferencesCommand();
    explicit UpdateCameraPreferencesCommand(const nlohmann::json& json);
    virtual ~UpdateCameraPreferencesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    const nlohmann::json& getPreferencesData() const;
    void                  setPreferencesData(const nlohmann::json& data);

  private:
    nlohmann::json preferencesData;
};
