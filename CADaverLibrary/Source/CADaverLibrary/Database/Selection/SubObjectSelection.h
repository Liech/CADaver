#pragma once

#include <nlohmann/json.hpp>
#include <string>

// Abstract base class for detailed subobject selection within a selected asset or subasset.
class SubObjectSelection
{
  public:
    SubObjectSelection();
    virtual ~SubObjectSelection();

    virtual std::string getType() const = 0;

    virtual nlohmann::json toJson() const                          = 0;
    virtual void           fromJson(const nlohmann::json& jsonObject) = 0;
};
