#pragma once

class PreferencesWindow;

// Renders the main file menu and delegates file open operations to GatedDatabase commands.
class FileMenu
{
  public:
    FileMenu();
    virtual ~FileMenu();

    void render(PreferencesWindow& preferencesWindow, bool& quitRequested);

  private:
    void openStlFileDialog();
    void openStepFileDialog();
    void openSvoFileDialog();
};
