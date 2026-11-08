#include "UIElement/UIElementFactory.h"

#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "UIElement/Menu/Create/LandscapeMenuEntry.h"
#include "UIElement/Menu/Edit/PreferencesMenuEntry.h"
#include "UIElement/Menu/Edit/RedoMenuEntry.h"
#include "UIElement/Menu/Edit/UndoMenuEntry.h"
#include "UIElement/Menu/File/LoadStepMenuEntry.h"
#include "UIElement/Menu/File/LoadStlMenuEntry.h"
#include "UIElement/Menu/File/LoadSvoMenuEntry.h"
#include "UIElement/Menu/File/QuitMenuEntry.h"
#include "UIElement/Menu/Help/AboutMenuEntry.h"
#include "UIElement/Menu/Help/CommandsInfoMenuEntry.h"
#include "UIElement/Menu/Help/ConceptsInfoMenuEntry.h"
#include "UIElement/Menu/Help/WindowsInfoMenuEntry.h"
#include "UIElement/Radio/CameraModeRadioElement.h"
#include "UIElement/Radio/ColorThemeRadioElement.h"
#include "UIElement/Radio/CullModeRadioElement.h"
#include "UIElement/Radio/ProjectionRadioElement.h"
#include "UIElement/Radio/WireframeRadioElement.h"
#include "Windows/Windows.h"

#include <algorithm>

UIElementFactory::UIElementFactory() {}

UIElementFactory::~UIElementFactory() {}

void UIElementFactory::registerDefaults(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows, bool& quitRequested)
{
    registerElement("STL", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<LoadStlMenuEntry>(gatedDatabase, undoStack, windows); });
    registerElement("STEP", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<LoadStepMenuEntry>(gatedDatabase, undoStack, windows); });
    registerElement("SVO", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<LoadSvoMenuEntry>(gatedDatabase, undoStack, windows); });
    registerElement("Preferences", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<PreferencesMenuEntry>(gatedDatabase, undoStack, windows); });
    registerElement("Quit", [&gatedDatabase, &undoStack, &windows, &quitRequested]() { return std::make_shared<QuitMenuEntry>(gatedDatabase, undoStack, windows, quitRequested); });
    registerElement("Undo", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<UndoMenuEntry>(gatedDatabase, undoStack, windows); });
    registerElement("Redo", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<RedoMenuEntry>(gatedDatabase, undoStack, windows); });
    registerElement("Landscape", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<LandscapeMenuEntry>(gatedDatabase, undoStack, windows); });
    registerElement("About", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<AboutMenuEntry>(gatedDatabase, undoStack, windows); });
    registerElement("Commands", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<CommandsInfoMenuEntry>(gatedDatabase, undoStack, windows); });
    registerElement("Concepts", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<ConceptsInfoMenuEntry>(gatedDatabase, undoStack, windows); });
    registerElement("Windows", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<WindowsInfoMenuEntry>(gatedDatabase, undoStack, windows); });

    registerElement("Wireframe: Off", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<WireframeRadioElement>(0, gatedDatabase, undoStack, windows); });
    registerElement("Wireframe: Overlay", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<WireframeRadioElement>(1, gatedDatabase, undoStack, windows); });
    registerElement("Wireframe: Only", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<WireframeRadioElement>(2, gatedDatabase, undoStack, windows); });

    registerElement("Culling: None", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<CullModeRadioElement>(0, gatedDatabase, undoStack, windows); });
    registerElement("Culling: Front", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<CullModeRadioElement>(1, gatedDatabase, undoStack, windows); });
    registerElement("Culling: Back", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<CullModeRadioElement>(2, gatedDatabase, undoStack, windows); });

    registerElement("Camera Type: Arcball", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<CameraModeRadioElement>(0, gatedDatabase, undoStack, windows); });
    registerElement("Camera Type: Freecam", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<CameraModeRadioElement>(1, gatedDatabase, undoStack, windows); });

    registerElement("Projection: Perspective", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<ProjectionRadioElement>(false, gatedDatabase, undoStack, windows); });
    registerElement("Projection: Orthogonal", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<ProjectionRadioElement>(true, gatedDatabase, undoStack, windows); });

    registerElement("Color Theme: Dark", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<ColorThemeRadioElement>(0, gatedDatabase, undoStack, windows); });
    registerElement("Color Theme: Light", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<ColorThemeRadioElement>(1, gatedDatabase, undoStack, windows); });
    registerElement("Color Theme: Classic", [&gatedDatabase, &undoStack, &windows]() { return std::make_shared<ColorThemeRadioElement>(2, gatedDatabase, undoStack, windows); });
}

void UIElementFactory::registerElement(const std::string& type, std::function<std::shared_ptr<UIElement>()> creator)
{
    if (std::find(registeredOrder.begin(), registeredOrder.end(), type) == registeredOrder.end())
    {
        registeredOrder.push_back(type);
    }
    creators[type] = std::move(creator);
}

std::shared_ptr<UIElement> UIElementFactory::create(const std::string& type) const
{
    auto it = creators.find(type);
    if (it != creators.end() && it->second)
    {
        return it->second();
    }
    return nullptr;
}

std::vector<std::shared_ptr<UIElement>> UIElementFactory::getAllElements() const
{
    std::vector<std::shared_ptr<UIElement>> elements;
    for (const auto& type : registeredOrder)
    {
        auto it = creators.find(type);
        if (it != creators.end() && it->second)
        {
            auto elem = it->second();
            if (elem)
            {
                elements.push_back(std::move(elem));
            }
        }
    }
    return elements;
}

std::vector<std::string> UIElementFactory::getRegisteredTypes() const
{
    return registeredOrder;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIElementFactory/Defaults")
{
    GatedDatabase    db;
    UndoStack        us(db);
    Windows          windows;
    bool             quitFlag = false;
    UIElementFactory factory;
    factory.registerDefaults(db, us, windows, quitFlag);

    REQUIRE(factory.create("SVO") != nullptr);
    REQUIRE(factory.create("STL") != nullptr);
    REQUIRE(factory.create("STEP") != nullptr);
    REQUIRE(factory.create("Preferences") != nullptr);
    REQUIRE(factory.create("Quit") != nullptr);
    REQUIRE(factory.create("Undo") != nullptr);
    REQUIRE(factory.create("Redo") != nullptr);
    REQUIRE(factory.create("Landscape") != nullptr);
    REQUIRE(factory.create("About") != nullptr);
    REQUIRE(factory.create("Commands") != nullptr);
    REQUIRE(factory.create("Concepts") != nullptr);
    REQUIRE(factory.create("Windows") != nullptr);
    REQUIRE(factory.create("Wireframe: Overlay") != nullptr);
    REQUIRE(factory.create("Culling: Back") != nullptr);
    REQUIRE(factory.create("Camera Type: Freecam") != nullptr);
    REQUIRE(factory.create("Projection: Orthogonal") != nullptr);
    REQUIRE(factory.create("Color Theme: Light") != nullptr);
    REQUIRE(factory.create("Unknown") == nullptr);
    REQUIRE(factory.getAllElements().size() == 25);
}
#endif
