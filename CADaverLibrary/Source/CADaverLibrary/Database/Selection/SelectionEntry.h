#pragma once

#include "CADacomb/Util/GUID.h"

#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

class SubObjectSelection;

// Represents a selected asset or subasset GUID and all subobject selections within it.
class SelectionEntry
{
  public:
    SelectionEntry();
    virtual ~SelectionEntry();

    const CADacomb::GUID&                                 getTargetGuid() const;
    void                                                  setTargetGuid(const CADacomb::GUID& guid);
    std::vector<std::shared_ptr<const SubObjectSelection>> getSubObjects() const;
    void                                                  addSubObject(std::shared_ptr<SubObjectSelection> subObject);
    void                                                  removeSubObject(const std::string& type);
    void                                                  clearSubObjects();

    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& jsonObject);

  private:
    CADacomb::GUID                                   targetGuid;
    std::vector<std::shared_ptr<SubObjectSelection>> subObjects;
};
