#pragma once

#include "Database/Preferences/CameraPreferences.h"
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

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

    const nlohmann::json& getPreferencesData() const;
    void                  setPreferencesData(const nlohmann::json& data);

  private:
    nlohmann::json    preferencesData;
    CameraPreferences previousPreferences;
    CameraPreferences newPreferences;
};
