#pragma once

class PreferencesWindow;
class WindowContainer;

// Renders the main file menu and opens file load and preferences windows.
class FileMenu
{
  public:
    FileMenu();
    virtual ~FileMenu();

    void render(WindowContainer& windowContainer, PreferencesWindow& preferencesWindow, bool& quitRequested);
};
