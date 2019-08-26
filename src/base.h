#pragma once

#include <glm/glm.hpp>
#include <vector>

void AddBase(
    std::vector<glm::vec3> &points,
    std::vector<glm::ivec3> &triangles,
    const int w, const int h, const float z);
