#include "Utility/MatrixJson.h"

MatrixJson::MatrixJson() {}

MatrixJson::~MatrixJson() {}

nlohmann::json MatrixJson::toJson(const glm::dmat4& matrix)
{
    nlohmann::json json = nlohmann::json::array();
    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            json.push_back(matrix[column][row]);
        }
    }
    return json;
}

glm::dmat4 MatrixJson::fromJson(const nlohmann::json& json)
{
    glm::dmat4 matrix(1.0);
    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            matrix[column][row] = json.at(column * 4 + row).get<double>();
        }
    }
    return matrix;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("MatrixJson/RoundTrip")
{
    glm::dmat4 matrix = glm::translate(glm::dmat4(1.0), glm::dvec3(1.0, 2.0, 3.0)) * glm::rotate(glm::dmat4(1.0), 0.5, glm::dvec3(0.0, 0.0, 1.0));
    CHECK(MatrixJson::fromJson(MatrixJson::toJson(matrix)) == matrix);
}
#endif
