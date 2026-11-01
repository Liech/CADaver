#pragma once

#include "Windows/Window.h"

#include <string>
#include <utility>
#include <vector>

// UI window presenting architectural, volumetric, and file format concepts.
class ConceptsInfoWindow : public Window
{
  public:
    ConceptsInfoWindow();
    virtual ~ConceptsInfoWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

  private:
    void loadData();

    std::vector<std::pair<std::string, std::string>> entries;
    int                                              selectedIndex = 0;
    bool                                             loaded        = false;
};
