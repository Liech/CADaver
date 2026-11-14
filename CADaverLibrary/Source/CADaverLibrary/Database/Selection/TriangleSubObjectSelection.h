#pragma once

#include "Database/Selection/SubObjectSelection.h"

#include <cstddef>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <unordered_set>

// Subobject selection storing selected triangle indices for mesh-based assets.
class TriangleSubObjectSelection : public SubObjectSelection
{
  public:
    TriangleSubObjectSelection();
    virtual ~TriangleSubObjectSelection();

    virtual std::string getType() const override;

    virtual nlohmann::json toJson() const override;
    virtual void           fromJson(const nlohmann::json& jsonObject) override;

    void             add(size_t index);
    void             remove(size_t index);
    void             clear();
    bool             contains(size_t index) const;
    std::set<size_t> getIndices() const;

  private:
    std::unordered_set<size_t> triangleIndices;
};
