#pragma once

#include "Database/Selection/SubObjectSelection.h"

#include <cstddef>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <unordered_set>

// Subobject selection storing selected voxel indices for SVO-based assets.
class VoxelSubObjectSelection : public SubObjectSelection
{
  public:
    VoxelSubObjectSelection();
    virtual ~VoxelSubObjectSelection();

    virtual std::string getType() const override;

    virtual nlohmann::json toJson() const override;
    virtual void           fromJson(const nlohmann::json& jsonObject) override;

    void             add(size_t index);
    void             remove(size_t index);
    void             clear();
    bool             contains(size_t index) const;
    std::set<size_t> getIndices() const;

  private:
    std::unordered_set<size_t> voxelIndices;
};
