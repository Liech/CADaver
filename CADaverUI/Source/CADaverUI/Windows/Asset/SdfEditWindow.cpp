#include "Windows/Asset/SdfEditWindow.h"

#include "Execution/GatedDatabase.h"

#include <algorithm>
#include <cstring>
#include <imgui.h>

SdfEditWindow::SdfEditWindow()
  : assetComboBox("Sdf")
  , helperBuffer(16384, '\0')
  , expressionBuffer(16384, '\0')
{
}

SdfEditWindow::~SdfEditWindow() {}

std::string SdfEditWindow::getType() const
{
    return "SdfEdit";
}

std::string SdfEditWindow::getDescription() const
{
    return "Window: SdfEdit\n"
           "Title: Edit SDF\n"
           "Purpose: Live inspector and editor for modifying an existing SdfAsset's Slang formula and AABB bounds.\n\n"
           "Parameters:\n"
           "  - Target Asset: SDF asset from the project to inspect and edit\n"
           "  - Bounds Min / Bounds Max: AABB extents updated live on drag\n"
           "  - Helper (Slang): Transpilable helper functions\n"
           "  - Expression (Slang): SDF evaluation body computing output[0] from input[0..2]\n\n"
           "Dispatched Commands: GetSdfData, UpdateSdfAsset";
}

void SdfEditWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
    loadedAssetGuid = CADacomb::GUID("");
    hasLoaded       = false;
    errorMessage.clear();
}

void SdfEditWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(560.0f, 520.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Edit SDF##SdfEditWindow", &openState))
    {
        assetComboBox.render(*gatedDatabase);

        CADacomb::GUID selectedGuid = assetComboBox.getSelectedGuid();
        if (!selectedGuid.toString().empty() && selectedGuid != loadedAssetGuid)
        {
            loadFromAsset();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (!hasLoaded)
        {
            ImGui::TextDisabled("Select an SDF asset to edit.");
        }
        else
        {
            ImGui::Text("AABB Bounds (Live)");
            bool boundsChanged = false;
            if (ImGui::DragFloat3("Bounds Min", boundsMin, 0.05f))
            {
                boundsChanged = true;
            }
            if (ImGui::DragFloat3("Bounds Max", boundsMax, 0.05f))
            {
                boundsChanged = true;
            }
            if (boundsChanged)
            {
                applyUpdate();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Helper (Slang)");
            ImGui::InputTextMultiline("##SdfHelper", helperBuffer.data(), helperBuffer.size(), ImVec2(-1.0f, 120.0f), ImGuiInputTextFlags_AllowTabInput);

            ImGui::Spacing();
            ImGui::Text("Expression (Slang)");
            ImGui::InputTextMultiline("##SdfExpression", expressionBuffer.data(), expressionBuffer.size(), ImVec2(-1.0f, 160.0f), ImGuiInputTextFlags_AllowTabInput);

            ImGui::Spacing();
            if (ImGui::Button("Apply Formula", ImVec2(140.0f, 0.0f)))
            {
                applyUpdate();
            }
            ImGui::SameLine();
            if (ImGui::Button("Reload", ImVec2(100.0f, 0.0f)))
            {
                loadFromAsset();
            }

            if (!errorMessage.empty())
            {
                ImGui::Spacing();
                ImGui::PushTextWrapPos(0.0f);
                ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", errorMessage.c_str());
                ImGui::PopTextWrapPos();
            }
        }
    }
    ImGui::End();
}

void SdfEditWindow::loadFromAsset()
{
    if (!gatedDatabase)
    {
        return;
    }

    CADacomb::GUID selectedGuid = assetComboBox.getSelectedGuid();
    if (selectedGuid.toString().empty())
    {
        hasLoaded = false;
        return;
    }

    Polyglot::Node command = {
        {      "type",            "GetSdfData" },
        {   "version",                       1 },
        { "assetGuid", selectedGuid.toString() }
    };

    Polyglot::Node result = gatedDatabase->execute(command);
    if (result["type"].asString() == "GetSdfData")
    {
        sdf             = CADacomb::Sdf::fromJson(result["sdf"].toJson());
        loadedAssetGuid = selectedGuid;
        hasLoaded       = true;
        errorMessage.clear();
        syncBuffersFromSdf();
    }
    else
    {
        hasLoaded    = false;
        errorMessage = result["message"].asString();
    }
}

void SdfEditWindow::applyUpdate()
{
    if (!gatedDatabase || loadedAssetGuid.toString().empty())
    {
        return;
    }

    syncSdfFromBuffers();

    Polyglot::Node command = {
        {      "type",             "UpdateSdfAsset" },
        {   "version",                            1 },
        { "assetGuid",   loadedAssetGuid.toString() },
        {       "sdf", Polyglot::Node(sdf.toJson()) }
    };

    Polyglot::Node result = gatedDatabase->execute(command);
    if (result["type"].asString() == "Error")
    {
        errorMessage = result["message"].asString();
    }
    else
    {
        errorMessage.clear();
    }
}

void SdfEditWindow::syncBuffersFromSdf()
{
    const CADacomb::AABB3& aabb = sdf.getAABB();
    boundsMin[0]                = static_cast<float>(aabb.getMin().x);
    boundsMin[1]                = static_cast<float>(aabb.getMin().y);
    boundsMin[2]                = static_cast<float>(aabb.getMin().z);
    boundsMax[0]                = static_cast<float>(aabb.getMax().x);
    boundsMax[1]                = static_cast<float>(aabb.getMax().y);
    boundsMax[2]                = static_cast<float>(aabb.getMax().z);

    std::string helper     = sdf.getFormula().getHelper();
    std::string expression = sdf.getFormula().getExpression();

    if (helper.size() + 1 > helperBuffer.size())
    {
        helperBuffer.resize(helper.size() + 4096, '\0');
    }
    std::fill(helperBuffer.begin(), helperBuffer.end(), '\0');
    std::memcpy(helperBuffer.data(), helper.c_str(), helper.size());

    if (expression.size() + 1 > expressionBuffer.size())
    {
        expressionBuffer.resize(expression.size() + 4096, '\0');
    }
    std::fill(expressionBuffer.begin(), expressionBuffer.end(), '\0');
    std::memcpy(expressionBuffer.data(), expression.c_str(), expression.size());
}

void SdfEditWindow::syncSdfFromBuffers()
{
    CADacomb::Formula updatedFormula = sdf.getFormula();
    updatedFormula.setHelper(std::string(helperBuffer.data()));
    updatedFormula.setExpression(std::string(expressionBuffer.data()));
    sdf.setFormula(updatedFormula);

    glm::dvec3 minPt(std::min(boundsMin[0], boundsMax[0]), std::min(boundsMin[1], boundsMax[1]), std::min(boundsMin[2], boundsMax[2]));
    glm::dvec3 maxPt(std::max(boundsMin[0], boundsMax[0]), std::max(boundsMin[1], boundsMax[1]), std::max(boundsMin[2], boundsMax[2]));
    sdf.setAABB(CADacomb::AABB3(minPt, maxPt));
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SdfEditWindow/TypeAndDescription")
{
    SdfEditWindow window;
    CHECK(window.getType() == "SdfEdit");
    CHECK(!window.getDescription().empty());
}
#endif
