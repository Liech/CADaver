#pragma once

class GatedDatabase;
class PreferencesWindow;

// Renders the main file menu and delegates file open operations to GatedDatabase commands.
class FileMenu
{
  public:
    FileMenu();
    virtual ~FileMenu();

    void render(GatedDatabase& gatedDatabase, PreferencesWindow& preferencesWindow, bool& quitRequested);

  private:
    void openStlFileDialog(GatedDatabase& gatedDatabase);
    void openStepFileDialog(GatedDatabase& gatedDatabase);
    void openSvoFileDialog(GatedDatabase& gatedDatabase);
};
