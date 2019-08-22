#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

#include "heightmap.h"
#include "queue.h"
#include "triangle.h"

class Model {
public:
    Model(const std::shared_ptr<Heightmap> &heightmap);

    const std::vector<std::shared_ptr<Triangle>> &Triangles() const;

    float Error() const;

    void Step();

private:
    void AddTriangle(
        const glm::ivec2 a,
        const glm::ivec2 b,
        const glm::ivec2 c);

    void Legalize(const glm::ivec2 a, const glm::ivec2 b);

    std::shared_ptr<Heightmap> m_Heightmap;
    std::unordered_map<glm::ivec4, std::shared_ptr<Triangle>> m_Halfedges;
    Queue m_Queue;
};
