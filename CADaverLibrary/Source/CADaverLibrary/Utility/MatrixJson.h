#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

// Converts a glm::dmat4 to and from a flat column-major array of 16 doubles.
class MatrixJson
{
  public:
    MatrixJson();
    virtual ~MatrixJson();

    static nlohmann::json toJson(const glm::dmat4& matrix);
    static glm::dmat4     fromJson(const nlohmann::json& json);
};
