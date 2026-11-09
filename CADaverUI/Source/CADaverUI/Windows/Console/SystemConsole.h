#pragma once

// Platform abstraction managing host operating system console window allocation and visibility.
class SystemConsole
{
  public:
    SystemConsole();
    virtual ~SystemConsole();

    static void setVisible(bool visible);
    static bool isVisible();
    static void setAvailable(bool isAvailable);
    static bool isAvailable();
    static bool attachParent();

  private:
    static bool available;
};
