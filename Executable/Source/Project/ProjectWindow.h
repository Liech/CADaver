#pragma once

class Project;

class ProjectWindow
{
  public:
    ProjectWindow();
    virtual ~ProjectWindow();

    void render(Project& project);

    float getWidth() const;
    void  setWidth(float width);

  private:
    float width = 300.0f;
};
