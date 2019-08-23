#pragma once

#include <glm/glm.hpp>

bool Collinear(const glm::ivec2 p0, const glm::ivec2 p1, const glm::ivec2 p2);

bool InCircle(
    const glm::ivec2 a, const glm::ivec2 b, const glm::ivec2 c,
    const glm::ivec2 p);
