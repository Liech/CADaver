#pragma once

#include "CADacomb/Util/GUID.h"

#include <string>

class Project;

// Combo box component for selecting an asset from the project, optionally filtered by type.
class AssetComboBox
{
  public:
    AssetComboBox();
    explicit AssetComboBox(const std::string& filterTypeInput);
    virtual ~AssetComboBox();

    bool render(const Project& project);

    const CADacomb::GUID& getSelectedGuid() const;
    void                  setSelectedGuid(const CADacomb::GUID& guid);

    const std::string& getFilterType() const;
    void               setFilterType(const std::string& filterTypeInput);

  private:
    CADacomb::GUID selectedGuid;
    std::string    filterType;
};
