#include "Windows/Console/SystemConsole.h"

#include <iostream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

bool SystemConsole::available = true;

SystemConsole::SystemConsole() {}

SystemConsole::~SystemConsole() {}

void SystemConsole::setVisible(bool visible)
{
    if (!available)
    {
        return;
    }

#ifdef _WIN32
    if (visible)
    {
        if (!GetConsoleWindow())
        {
            if (AllocConsole())
            {
                FILE* dummy = nullptr;
                freopen_s(&dummy, "CONOUT$", "w", stdout);
                freopen_s(&dummy, "CONOUT$", "w", stderr);
                freopen_s(&dummy, "CONIN$", "r", stdin);
                std::cout.clear();
                std::cerr.clear();
                std::cin.clear();
            }
        }
        else
        {
            HWND console = GetConsoleWindow();
            if (console)
            {
                ShowWindow(console, SW_SHOW);
                ShowWindow(console, SW_RESTORE);

                HWND root = GetAncestor(console, GA_ROOT);
                if (root && root != console)
                {
                    ShowWindow(root, SW_SHOW);
                    ShowWindow(root, SW_RESTORE);
                }
            }
        }
    }
    else
    {
        HWND console = GetConsoleWindow();
        if (console)
        {
            HWND root = GetAncestor(console, GA_ROOT);
            ShowWindow(console, SW_HIDE);
            if (root && root != console)
            {
                ShowWindow(root, SW_HIDE);
                PostMessage(root, WM_CLOSE, 0, 0);
            }

            PostMessage(console, WM_CLOSE, 0, 0);
            FreeConsole();
        }
    }
#else
    (void)visible;
#endif
}

bool SystemConsole::isVisible()
{
#ifdef _WIN32
    HWND console = GetConsoleWindow();
    if (!console)
    {
        return false;
    }

    return IsWindowVisible(console) != FALSE;
#else
    return false;
#endif
}

void SystemConsole::setAvailable(bool isAvailable)
{
    available = isAvailable;
}

bool SystemConsole::isAvailable()
{
#ifdef _WIN32
    return available;
#else
    return false;
#endif
}

bool SystemConsole::attachParent()
{
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        FILE* dummy = nullptr;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
        freopen_s(&dummy, "CONIN$", "r", stdin);
        std::cout.clear();
        std::cerr.clear();
        std::cin.clear();
        return true;
    }

    return false;
#else
    return true;
#endif
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SystemConsole/Availability")
{
    SystemConsole::setAvailable(true);
    CHECK(SystemConsole::isAvailable() == true);
    SystemConsole::setAvailable(false);
    CHECK(SystemConsole::isAvailable() == false);
    SystemConsole::setAvailable(true);
}

TEST_CASE("SystemConsole/VisibilityToggle")
{
    bool original = SystemConsole::isVisible();
    if (original)
    {
        SystemConsole::setVisible(false);
        CHECK(SystemConsole::isVisible() == false);
        SystemConsole::setVisible(true);
        CHECK(SystemConsole::isVisible() == true);
    }
}
#endif
