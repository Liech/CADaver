#include "Windows/Generator/BaseShapeGeneratorWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>
#include <vector>

BaseShapeGeneratorWindow::BaseShapeGeneratorWindow() {}

BaseShapeGeneratorWindow::~BaseShapeGeneratorWindow() {}

std::string BaseShapeGeneratorWindow::getType() const
{
    return "BaseShapeGenerator";
}

std::string BaseShapeGeneratorWindow::getDescription() const
{
    return "Window: BaseShapeGenerator\n"
           "Title: Base Shape Generator\n"
           "Purpose: Configures parametric geometry options and generates a TriangulationAsset.\n\n"
           "Dispatched Command: GenerateBaseShape (async)";
}

void BaseShapeGeneratorWindow::startGenerate()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    static const std::vector<std::string> shapeIds = {
        "cube",
        "box",
        "pyramid",
        "cylinder",
        "cone",
        "frustum",
        "sphere",
        "torus",
        "capsule",
        "prism",
        "octahedron",
        "cubeWithHole",
        "lBracket",
        "steppedBlock",
        "plateWithHoles"
    };

    std::string shapeId = shapeIds[static_cast<size_t>(selectedShapeIndex)];

    Polyglot::Node command = {
        {          "type", "GenerateBaseShape" },
        {       "version",                   1 },
        {         "shape",             shapeId },
        {     "assetName",           assetName },
        {         "width",               width },
        {         "depth",               depth },
        {        "height",              height },
        {        "length",              length },
        {     "thickness",           thickness },
        {        "radius",              radius },
        {  "bottomRadius",        bottomRadius },
        {     "topRadius",           topRadius },
        {   "majorRadius",         majorRadius },
        {   "minorRadius",         minorRadius },
        {    "holeRadius",          holeRadius },
        {      "segments",            segments },
        {         "rings",               rings },
        { "majorSegments",       majorSegments },
        { "minorSegments",       minorSegments },
        {         "sides",               sides }
    };

    GatedDatabase* db = gatedDatabase;
    defer->defer(
      [db, command]()
      {
          db->executeAsync(command);
      });
}

void BaseShapeGeneratorWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    static const char* shapeLabels[] = {
        "Cube",
        "Box",
        "Pyramid",
        "Cylinder",
        "Cone",
        "Frustum",
        "Sphere",
        "Torus",
        "Capsule",
        "Prism",
        "Octahedron",
        "Cube With Hole",
        "L-Bracket",
        "Stepped Block",
        "Plate With Holes"
    };

    static const std::vector<std::string> shapeIds = {
        "cube",
        "box",
        "pyramid",
        "cylinder",
        "cone",
        "frustum",
        "sphere",
        "torus",
        "capsule",
        "prism",
        "octahedron",
        "cubeWithHole",
        "lBracket",
        "steppedBlock",
        "plateWithHoles"
    };

    ImGui::SetNextWindowSize(ImVec2(420.0f, 380.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Base Shape Generator", &openState))
    {
        handleEscapeClose();

        if (ImGui::Combo("Shape", &selectedShapeIndex, shapeLabels, 15))
        {
            assetName = shapeIds[static_cast<size_t>(selectedShapeIndex)];
        }

        char nameBuffer[128];
        strncpy_s(nameBuffer, sizeof(nameBuffer), assetName.c_str(), _TRUNCATE);
        if (ImGui::InputText("Asset Name", nameBuffer, sizeof(nameBuffer)))
        {
            assetName = std::string(nameBuffer);
        }

        const std::string& shape = shapeIds[static_cast<size_t>(selectedShapeIndex)];
        if (shape == "box")
        {
            ImGui::InputFloat("Width", &width, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Depth", &depth, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Height", &height, 0.1f, 1.0f, "%.3f");
        }
        else if (shape == "cylinder" || shape == "cone")
        {
            ImGui::InputInt("Segments", &segments);
            ImGui::InputFloat("Radius", &radius, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Height", &height, 0.1f, 1.0f, "%.3f");
        }
        else if (shape == "frustum")
        {
            ImGui::InputInt("Segments", &segments);
            ImGui::InputFloat("Bottom Radius", &bottomRadius, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Top Radius", &topRadius, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Height", &height, 0.1f, 1.0f, "%.3f");
        }
        else if (shape == "sphere")
        {
            ImGui::InputInt("Segments", &segments);
            ImGui::InputInt("Rings", &rings);
            ImGui::InputFloat("Radius", &radius, 0.1f, 1.0f, "%.3f");
        }
        else if (shape == "torus")
        {
            ImGui::InputInt("Major Segments", &majorSegments);
            ImGui::InputInt("Minor Segments", &minorSegments);
            ImGui::InputFloat("Major Radius", &majorRadius, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Minor Radius", &minorRadius, 0.05f, 0.5f, "%.3f");
        }
        else if (shape == "capsule")
        {
            ImGui::InputInt("Segments", &segments);
            ImGui::InputInt("Rings", &rings);
            ImGui::InputFloat("Radius", &radius, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Height", &height, 0.1f, 1.0f, "%.3f");
        }
        else if (shape == "prism")
        {
            ImGui::InputInt("Sides", &sides);
            ImGui::InputFloat("Radius", &radius, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Height", &height, 0.1f, 1.0f, "%.3f");
        }
        else if (shape == "octahedron")
        {
            ImGui::InputFloat("Radius", &radius, 0.1f, 1.0f, "%.3f");
        }
        else if (shape == "cubeWithHole")
        {
            ImGui::InputInt("Segments", &segments);
            ImGui::InputFloat("Hole Radius", &holeRadius, 0.05f, 0.1f, "%.3f");
        }
        else if (shape == "lBracket")
        {
            ImGui::InputFloat("Length", &length, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Width", &width, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Height", &height, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Thickness", &thickness, 0.05f, 0.2f, "%.3f");
        }
        else if (shape == "steppedBlock")
        {
            ImGui::InputFloat("Length", &length, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Width", &width, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Height", &height, 0.1f, 1.0f, "%.3f");
        }
        else if (shape == "plateWithHoles")
        {
            ImGui::InputInt("Segments", &segments);
            ImGui::InputFloat("Length", &length, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Width", &width, 0.1f, 1.0f, "%.3f");
            ImGui::InputFloat("Thickness", &thickness, 0.05f, 0.2f, "%.3f");
            ImGui::InputFloat("Hole Radius", &holeRadius, 0.05f, 0.2f, "%.3f");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Generate Shape", ImVec2(160.0f, 0.0f)))
        {
            startGenerate();
            close();
        }
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("BaseShapeGeneratorWindow/InitialState")
{
    BaseShapeGeneratorWindow window;
    CHECK(window.getType() == "BaseShapeGenerator");
    CHECK_FALSE(window.getDescription().empty());
    CHECK_FALSE(window.isOpen());

    window.open();
    CHECK(window.isOpen());

    window.close();
    CHECK_FALSE(window.isOpen());
}
#endif
