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
    return "TextNoteEdit";
}

std::string TextNoteEditWindow::getDescription() const
{
    return "Window: TextNoteEdit\n"
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
        handleEscapeClose();
        assetComboBox.render(*gatedDatabase);
        CADacomb::GUID selectedAssetGuid = assetComboBox.getSelectedGuid();

        if (selectedAssetGuid.toString().empty())
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No assets available in project.");
            ImGui::End();
            return;
        }

        Polyglot::Node notesResponse = gatedDatabase->execute({
          {      "type",                   "GetNotes" },
          {   "version",                            1 },
          { "assetGuid", selectedAssetGuid.toString() }
        });
        nlohmann::json notesJson     = notesResponse.toJson();
        if (notesJson.value("status", "") != "Success")
        {
            ImGui::End();
            return;
        }

        const auto& notes = notesJson["notes"];
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
                      Polyglot::Node command = {
                          {      "type",                    "AddNote" },
                          {   "version",                            1 },
                          { "assetGuid", selectedAssetGuid.toString() },
                          {      "name",                       "Note" },
                          {      "text",                           "" }
                      };
                      gatedDatabase->execute(command);
                  });
                needsSync = true;
            }

            ImGui::End();
            return;
        }

        int activeNoteIdx = -1;
        for (size_t i = 0; i < notes.size(); i++)
        {
            if (CADacomb::GUID(notes[i].value("componentGuid", "")) == currentComponentGuid)
            {
                activeNoteIdx = static_cast<int>(i);
                break;
            }
        }

        if (activeNoteIdx < 0)
        {
            activeNoteIdx        = 0;
            currentComponentGuid = CADacomb::GUID(notes[0].value("componentGuid", ""));
            needsSync            = true;
        }

        const auto& activeNote      = notes[static_cast<size_t>(activeNoteIdx)];
        std::string activeNoteName  = activeNote.value("name", "");
        std::string previewNoteName = activeNoteName.empty() ? "Untitled Note" : activeNoteName;
        if (ImGui::BeginCombo("Note", previewNoteName.c_str()))
        {
            for (const auto& note : notes)
            {
                CADacomb::GUID noteGuid   = CADacomb::GUID(note.value("componentGuid", ""));
                std::string    noteName   = note.value("name", "");
                bool           isSelected = (noteGuid == currentComponentGuid);
                std::string    itemLabel  = (noteName.empty() ? "Untitled Note" : noteName) + "##" + noteGuid.toString();
                if (ImGui::Selectable(itemLabel.c_str(), isSelected))
                {
                    currentComponentGuid = noteGuid;
                    needsSync            = true;
                }
            }
            ImGui::EndCombo();
        }

        if (needsSync)
        {
            for (const auto& note : notes)
            {
                if (CADacomb::GUID(note.value("componentGuid", "")) == currentComponentGuid)
                {
                    cachedName = note.value("name", "");
                    cachedText = note.value("text", "");
                    break;
                }
            }
            needsSync = false;
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
            CADacomb::GUID compToUpdate = currentComponentGuid;
            std::string    updatedName  = cachedName;
            std::string    updatedText  = cachedText;
            defer->defer(
              [this, selectedAssetGuid, compToUpdate, updatedName, updatedText]()
              {
                  Polyglot::Node command = {
                      {          "type",                 "UpdateNote" },
                      {       "version",                            1 },
                      {     "assetGuid", selectedAssetGuid.toString() },
                      { "componentGuid",      compToUpdate.toString() },
                      {          "name",                  updatedName },
                      {          "text",                  updatedText }
                  };
                  gatedDatabase->execute(command);
              });
        }

        ImGui::SameLine();
        if (ImGui::Button("New", ImVec2(90.0f, 0.0f)))
        {
            defer->defer(
              [this, selectedAssetGuid]()
              {
                  Polyglot::Node command = {
                      {      "type",                    "AddNote" },
                      {   "version",                            1 },
                      { "assetGuid", selectedAssetGuid.toString() },
                      {      "name",                   "New Note" },
                      {      "text",                           "" }
                  };
                  gatedDatabase->execute(command);
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
                  Polyglot::Node command = {
                      {          "type",            "RemoveComponent" },
                      {       "version",                            1 },
                      { "componentGuid",      compToDelete.toString() },
                      {     "assetGuid", selectedAssetGuid.toString() }
                  };
                  gatedDatabase->execute(command);
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
    CHECK(window.getType() == "TextNoteEdit");
    CHECK(window.getDescription().find("Window: TextNoteEdit") != std::string::npos);
    CHECK(window.getDescription().find("AddNote, RemoveComponent") != std::string::npos);
}
#endif
