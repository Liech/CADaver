#pragma once

class PreferencesWindow;
class Windows;

// Renders the main file menu and opens file load and preferences windows.
class FileMenu
{
  public:
    FileMenu();
    virtual ~FileMenu();

    void render(Windows& windows, PreferencesWindow& preferencesWindow, bool& quitRequested);
};
