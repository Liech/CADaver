#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"
#include "Polyglot/Node.h"

#include <memory>
#include <string>

class TextureComponent;

// Command to attach a TextureComponent to a TriangulationAsset from a TextureAsset or image source.
class AddTextureComponentCommand : public Command
{
  public:
    AddTextureComponentCommand();
    virtual ~AddTextureComponentCommand();

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
    CADacomb::GUID                    assetGuid;
    CADacomb::GUID                    textureAssetGuid;
    bool                              hasTextureAssetGuid = false;
    Polyglot::Node                    source;
    bool                              hasSource = false;
    std::string                       name;
    std::shared_ptr<TextureComponent> addedComponent;
};
