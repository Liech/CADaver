#include "Database/Component/TextNoteComponent.h"

TextNoteComponent::TextNoteComponent() {}

TextNoteComponent::TextNoteComponent(const std::string& textInput)
  : text(textInput)
{
}

TextNoteComponent::~TextNoteComponent() {}

std::string TextNoteComponent::getType() const
{
    return "TextNote";
}

std::string TextNoteComponent::getDescription() const
{
    return "Component: TextNote\n"
           "Purpose: Attaches user notes, documentation, and metadata annotations to an asset.\n"
           "Compatibility: Compatible with all asset types.\n\n"
           "Data Members:\n"
           "  - Title (name): Short label or heading identifying the note\n"
           "  - Text: Multi-line textual content or annotations";
}

size_t TextNoteComponent::getSizeInBytes() const
{
    return sizeof(TextNoteComponent) + text.capacity();
}

bool TextNoteComponent::isCompatible(const std::string&) const
{
    return true;
}

nlohmann::json TextNoteComponent::toDataJson() const
{
    return {
        { "Version",    1 },
        {    "text", text }
    };
}

void TextNoteComponent::fromDataJson(const nlohmann::json& jsonObject)
{
    jsonObject.at("Version").get<int>();
    text = jsonObject.at("text").get<std::string>();
    markModified();
}

const std::string& TextNoteComponent::getText() const
{
    return text;
}

void TextNoteComponent::setText(const std::string& newText)
{
    text = newText;
    markModified();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TextNoteComponent/BasicProperties")
{
    TextNoteComponent note("Sample note");
    CHECK(note.getType() == "TextNote");
    CHECK(note.getText() == "Sample note");
    CHECK(note.isCompatible("Triangulation") == true);
    CHECK(note.isCompatible("Voxel") == true);
    CHECK(note.isCompatible("CAD") == true);

    note.setText("Updated");
    CHECK(note.getText() == "Updated");
    CHECK(note.getSizeInBytes() >= sizeof(TextNoteComponent));
}
#endif
