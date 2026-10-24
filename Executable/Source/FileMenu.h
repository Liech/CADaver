#pragma once

class PreferencesWindow;
class Project;

class FileMenu
{
  public:
    FileMenu();
    virtual ~FileMenu();

    void render(Project& project, PreferencesWindow& preferencesWindow, bool& quitRequested);

  private:
    void openStlFileDialog(Project& project);
    void openStepFileDialog(Project& project);
};
