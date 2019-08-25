#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "heightmap.h"

class Triangulator {
public:
    Triangulator(const std::shared_ptr<Heightmap> &heightmap);

    float Error() const;

    void Step();

    void Dump() const;

    const std::vector<glm::vec3> Points() const {
        std::vector<glm::vec3> points;
        points.reserve(m_Points.size());
        for (const glm::ivec2 &p : m_Points) {
            points.emplace_back(p.x, p.y, m_Heightmap->At(p.x, p.y) * 200);
        }
        return points;
    }

    std::vector<glm::ivec3> Triangles() const {
        std::vector<glm::ivec3> triangles;
        triangles.reserve(m_Queue.size());
        for (const int i : m_Queue) {
            triangles.emplace_back(
                m_Triangles[i * 3 + 2],
                m_Triangles[i * 3 + 1],
                m_Triangles[i * 3 + 0]);
        }
        return triangles;
    }

private:
    int AddPoint(const glm::ivec2 point);

    int AddTriangle(
        const int a, const int b, const int c,
        const int ab, const int bc, const int ca);

    void Legalize(const int a);

    void QueuePush(const int t);
    int QueuePop();
    int QueuePopBack();
    void QueueRemove(const int t);
    bool QueueLess(const int i, const int j) const;
    void QueueSwap(const int i, const int j);
    void QueueUp(const int j0);
    bool QueueDown(const int i0, const int n);

    std::shared_ptr<Heightmap> m_Heightmap;

    std::vector<glm::ivec2> m_Points;

    std::vector<int> m_Triangles;
    std::vector<int> m_Halfedges;

    std::vector<glm::ivec2> m_Candidates;
    std::vector<float> m_Errors;
    std::vector<int> m_QueueIndexes;

    std::vector<int> m_Queue;
};
