#pragma once

#include "Database/AssetComponent.h"

#include <string>

// Text note component attached to an Asset for user notes and metadata.
class TextNoteComponent : public AssetComponent
{
  public:
    TextNoteComponent();
    explicit TextNoteComponent(const std::string& text);
    virtual ~TextNoteComponent();

    std::string getType() const override;
    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;
    bool        isCompatible(const std::string& assetType) const override;

    const std::string& getText() const;
    void               setText(const std::string& text);

  private:
    std::string text;
};
