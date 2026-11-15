#pragma once

#include "Windows/Window.h"

#include <string>

// Window for configuring and generating parametric base triangulation shapes.
class BaseShapeGeneratorWindow : public Window
{
  public:
    BaseShapeGeneratorWindow();
    virtual ~BaseShapeGeneratorWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

  private:
    void startGenerate();

    int         selectedShapeIndex = 0;
    std::string assetName          = "cube";
    float       width              = 1.0f;
    float       depth              = 1.0f;
    float       height             = 1.0f;
    float       length             = 2.0f;
    float       thickness          = 0.3f;
    float       radius             = 0.5f;
    float       bottomRadius       = 1.0f;
    float       topRadius          = 0.5f;
    float       majorRadius        = 1.0f;
    float       minorRadius        = 0.25f;
    float       holeRadius         = 0.3f;
    int         segments           = 32;
    int         rings              = 16;
    int         majorSegments      = 32;
    int         minorSegments      = 16;
    int         sides              = 6;
};
