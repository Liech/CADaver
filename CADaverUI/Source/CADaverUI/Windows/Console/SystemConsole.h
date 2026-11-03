#pragma once

// Platform abstraction managing host operating system console window allocation and visibility.
class SystemConsole
{
  public:
    SystemConsole();
    virtual ~SystemConsole();

    static void setVisible(bool visible);
    static bool isVisible();
    static bool isAvailable();
};
