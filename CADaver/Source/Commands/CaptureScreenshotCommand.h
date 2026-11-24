#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>
#include <string>

class Renderer;
class TextureAsset;

// Captures the current rendered viewport image and adds it as a TextureAsset to the project.
class CaptureScreenshotCommand : public Command
{
  public:
    CaptureScreenshotCommand(Renderer& renderer);
    virtual ~CaptureScreenshotCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    void           setInput(const Polyglot::Node& node) override;
    nlohmann::json execute(Database& database) override;

    bool   isUndoable() const override;
    void   undo(Database& database) override;
    void   redo(Database& database) override;
    size_t getUndoSize() const override;

  private:
    Renderer&                     renderer;
    std::string                   assetName = "Screenshot";
    std::shared_ptr<TextureAsset> createdAsset;
    CADacomb::GUID                createdAssetGuid;
};
