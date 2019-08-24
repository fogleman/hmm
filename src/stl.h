#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

void SaveBinarySTL(
    const std::string &path,
    const std::vector<glm::vec3> &points,
    const std::vector<glm::ivec3> &triangles);
