#pragma once

#include <memory>
#include <nlohmann/json.hpp>

class SubObjectSelection;

// Factory for deserializing SubObjectSelection instances from JSON.
class SubObjectSelectionFactory
{
  public:
    SubObjectSelectionFactory();
    virtual ~SubObjectSelectionFactory();

    static std::shared_ptr<SubObjectSelection> fromJson(const nlohmann::json& jsonObject);
};
