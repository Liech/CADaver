#include "Windows/Window.h"

#include <imgui.h>

Window::Window() {}

Window::~Window() {}

void Window::setInput(GatedDatabase& gatedDatabaseInput, Defer& deferInput, Windows& windowsInput)
{
    gatedDatabase = &gatedDatabaseInput;
    defer         = &deferInput;
    windows       = &windowsInput;
}

bool Window::isOpen() const
{
    return openState;
}

void Window::open()
{
    openState   = true;
    shouldFocus = true;
}

void Window::close()
{
    openState   = false;
    shouldFocus = false;
}

bool Window::consumeFocus()
{
    if (shouldFocus)
    {
        shouldFocus = false;
        return true;
    }
    return false;
}

void Window::handleEscapeClose()
{
    if (openState && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        close();
    }
}
