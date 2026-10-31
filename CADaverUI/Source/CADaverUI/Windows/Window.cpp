#include "Windows/Window.h"

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
