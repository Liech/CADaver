#pragma once

class Windows;

// Renders the main Create top menu and delegates opening creation windows.
class CreateMenu
{
  public:
    CreateMenu();
    virtual ~CreateMenu();

    void render(Windows& windows);
};
