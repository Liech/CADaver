#pragma once

#include "CADacomb/Util/GUID.h"
#include "CADacomb/Volume/SVO/Generator/LandscapeConfig.h"
#include "CADacomb/Volume/SVO/SVO.h"
#include "UI/Component/ModalProgressBar.h"
#include "UI/Component/VoxelResolutionComboBox.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

// Window for configuring and generating volumetric landscapes asynchronously with progress feedback.
class LandscapeGeneratorWindow
{
  public:
    LandscapeGeneratorWindow();
    virtual ~LandscapeGeneratorWindow();

    void render();
    void startGenerate();

    void open();
    void close();
    bool isOpen() const;
    void setOpen(bool open);

    bool isGenerating() const;

    CADacomb::LandscapeConfig getConfig() const;
    int                       getResolution() const;

  private:
    void commitAsset(std::unique_ptr<CADacomb::SVO<bool>> svo);

    VoxelResolutionComboBox              resolutionComboBox;
    ModalProgressBar                     modalProgressBar;
    std::unique_ptr<std::thread>         workerThread;
    std::atomic<bool>                    generatingState = false;
    CADacomb::GUID                       taskGuid;
    std::unique_ptr<CADacomb::SVO<bool>> pendingSVO;
    std::mutex                           resultMutex;

    int         resolution    = 64;
    int         octaves       = 4;
    float       frequency     = 0.05f;
    float       baseHeight    = 0.4f;
    int         seed          = 1337;
    bool        caves         = false;
    float       caveThreshold = 0.12f;
    std::string assetName     = "Landscape";

    double generationTimeMs = 0.0;
    size_t nodeCount        = 0;
    bool   hasNewSVO        = false;
    bool   openState        = false;
};
