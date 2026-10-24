#pragma once

class Renderer;

// Abstract base class for rendering project assets into a Renderer.
class AssetRenderer
{
  public:
    AssetRenderer();
    virtual ~AssetRenderer();

    virtual void render(Renderer& renderer, bool isSelected = false) = 0;
};
