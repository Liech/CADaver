#pragma once

#include "CADacomb/Util/GUID.h"
#include "Database/Selection/SelectionEntry.h"

#include <map>
#include <nlohmann/json.hpp>
#include <vector>

// Headless selection storage managing selected asset or subasset entries and subobject details.
class Selection
{
  public:
    Selection();
    virtual ~Selection();

    void select(const SelectionEntry& entry);
    void addSelection(const SelectionEntry& entry);
    void deselect(const CADacomb::GUID& targetGuid);
    void clear();

    bool                        isSelected(const CADacomb::GUID& targetGuid) const;
    std::vector<SelectionEntry> getEntries() const;

    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& jsonObject);

  private:
    std::map<CADacomb::GUID, SelectionEntry> entries;
};
