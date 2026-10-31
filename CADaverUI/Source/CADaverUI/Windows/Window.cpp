#include "Windows/Window.h"

Window::Window() {}

Window::~Window() {}

void Window::setInput(GatedDatabase& gatedDatabaseInput, Defer& deferInput)
{
    gatedDatabase = &gatedDatabaseInput;
    defer         = &deferInput;
}
