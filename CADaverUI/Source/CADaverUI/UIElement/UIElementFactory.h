#pragma once

#include "UIElement/UIElement.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class GatedDatabase;
class UndoStack;
class Windows;

// Factory registering and producing interactive UI elements and menu entries.
class UIElementFactory
{
  public:
    UIElementFactory();
    virtual ~UIElementFactory();

    void registerDefaults(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows, bool& quitRequested);
    void registerElement(const std::string& type, std::function<std::shared_ptr<UIElement>()> creator);

    std::shared_ptr<UIElement>              create(const std::string& type) const;
    std::vector<std::shared_ptr<UIElement>> getAllElements() const;
    std::vector<std::string>                getRegisteredTypes() const;

  private:
    std::map<std::string, std::function<std::shared_ptr<UIElement>()>> creators;
    std::vector<std::string>                                           registeredOrder;
};
