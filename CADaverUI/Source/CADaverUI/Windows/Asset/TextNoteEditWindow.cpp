#include "Windows/Asset/TextNoteEditWindow.h"

#include "Database/Asset.h"
#include "Database/Component/TextNoteComponent.h"
#include "Database/Database.h"
#include "Database/Project.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <cstring>
#include <imgui.h>

TextNoteEditWindow::TextNoteEditWindow() {}

TextNoteEditWindow::~TextNoteEditWindow() {}

std::string TextNoteEditWindow::getType() const
{
    return "TextNoteEditWindow";
}

std::string TextNoteEditWindow::getDescription() const
{
    return "Window: TextNoteEditWindow\n"
           "Title: Edit Text Note\n"
           "Purpose: Self-sufficient dialog for selecting project assets and viewing, authoring, and managing their attached text notes and metadata annotations.\n\n"
           "Parameters:\n"
           "  - Target Asset: ComboBox selecting the parent asset within the project\n"
           "  - Note: ComboBox selecting the active note component attached to the chosen asset\n"
           "  - Title: Input field defining the note label\n"
           "  - Body: Multi-line text editor for authoring note content\n\n"
           "Dispatched Commands: AddNote, RemoveComponent";
}

void TextNoteEditWindow::setTarget(const CADacomb::GUID& targetAssetGuid, const CADacomb::GUID& targetComponentGuid)
{
    assetComboBox.setSelectedGuid(targetAssetGuid);
    currentComponentGuid = targetComponentGuid;
    needsSync            = true;
}

void TextNoteEditWindow::render()
{
    if (!openState || !gatedDatabase || !defer)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500.0f, 440.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Edit Text Note", &openState))
    {
        const auto& project = gatedDatabase->get().getProject();

        assetComboBox.render(project);
        CADacomb::GUID selectedAssetGuid = assetComboBox.getSelectedGuid();

        if (selectedAssetGuid.toString().empty())
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No assets available in project.");
            ImGui::End();
            return;
        }

        auto asset = project.getAsset(selectedAssetGuid);
        if (!asset)
        {
            ImGui::End();
            return;
        }

        auto notes = asset->getAttachments<TextNoteComponent>();

        if (notes.empty())
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No notes attached to this asset.");
            ImGui::Spacing();

            if (ImGui::Button("Add Note", ImVec2(120.0f, 0.0f)))
            {
                defer->defer(
                  [this, selectedAssetGuid]()
                  {
                      gatedDatabase->execute({
                        {      "type",                    "AddNote" },
                        {   "version",                            1 },
                        { "assetGuid", selectedAssetGuid.toString() },
                        {      "name",                       "Note" },
                        {      "text",                           "" }
                      });
                  });
                needsSync = true;
            }

            ImGui::End();
            return;
        }

        std::shared_ptr<TextNoteComponent> activeNote = nullptr;
        for (const auto& note : notes)
        {
            if (note && note->getGuid() == currentComponentGuid)
            {
                activeNote = note;
                break;
            }
        }

        if (!activeNote)
        {
            activeNote           = notes.front();
            currentComponentGuid = activeNote->getGuid();
            needsSync            = true;
        }

        std::string previewNoteName = activeNote->getName().empty() ? "Untitled Note" : activeNote->getName();
        if (ImGui::BeginCombo("Note", previewNoteName.c_str()))
        {
            for (const auto& note : notes)
            {
                if (!note)
                {
                    continue;
                }

                bool        isSelected = (note->getGuid() == currentComponentGuid);
                std::string itemLabel  = (note->getName().empty() ? "Untitled Note" : note->getName()) + "##" + note->getGuid().toString();
                if (ImGui::Selectable(itemLabel.c_str(), isSelected))
                {
                    currentComponentGuid = note->getGuid();
                    activeNote           = note;
                    needsSync            = true;
                }
            }
            ImGui::EndCombo();
        }

        if (needsSync && activeNote)
        {
            cachedName = activeNote->getName();
            cachedText = activeNote->getText();
            needsSync  = false;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        char nameBuf[128];
        std::strncpy(nameBuf, cachedName.c_str(), sizeof(nameBuf));
        nameBuf[sizeof(nameBuf) - 1] = '\0';
        if (ImGui::InputText("Title", nameBuf, sizeof(nameBuf)))
        {
            cachedName = nameBuf;
        }

        ImGui::Spacing();

        char textBuf[4096];
        std::strncpy(textBuf, cachedText.c_str(), sizeof(textBuf));
        textBuf[sizeof(textBuf) - 1] = '\0';
        if (ImGui::InputTextMultiline("##NoteBody", textBuf, sizeof(textBuf), ImVec2(-1.0f, -40.0f)))
        {
            cachedText = textBuf;
        }

        ImGui::Spacing();

        if (ImGui::Button("Save", ImVec2(90.0f, 0.0f)))
        {
            if (activeNote)
            {
                activeNote->setName(cachedName);
                activeNote->setText(cachedText);
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("New", ImVec2(90.0f, 0.0f)))
        {
            defer->defer(
              [this, selectedAssetGuid]()
              {
                  gatedDatabase->execute({
                    {      "type",                    "AddNote" },
                    {   "version",                            1 },
                    { "assetGuid", selectedAssetGuid.toString() },
                    {      "name",                   "New Note" },
                    {      "text",                           "" }
                  });
              });
            needsSync = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("Delete", ImVec2(100.0f, 0.0f)))
        {
            CADacomb::GUID compToDelete = currentComponentGuid;
            defer->defer(
              [this, compToDelete, selectedAssetGuid]()
              {
                  gatedDatabase->execute({
                    {          "type",            "RemoveComponent" },
                    {       "version",                            1 },
                    { "componentGuid",      compToDelete.toString() },
                    {     "assetGuid", selectedAssetGuid.toString() }
                  });
              });
            currentComponentGuid = CADacomb::GUID("");
            needsSync            = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(90.0f, 0.0f)))
        {
            close();
        }
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TextNoteEditWindow/TypeAndDescription")
{
    TextNoteEditWindow window;
    CHECK(window.getType() == "TextNoteEditWindow");
    CHECK(window.getDescription().find("Window: TextNoteEditWindow") != std::string::npos);
    CHECK(window.getDescription().find("AddNote, RemoveComponent") != std::string::npos);
}
#endif
