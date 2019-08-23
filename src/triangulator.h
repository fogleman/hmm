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

private:
    int AddPoint(const glm::ivec2 point);

    int AddTriangle(
        const int a, const int b, const int c,
        const int ab, const int bc, const int ca);

    void UnlinkTriangle(const int t);

    void Link(const int a, const int b);

    void Legalize(const int a);

    void QueuePush(const int t);
    int QueuePop();
    int QueuePopBack();
    void QueueRemove(const int t);
    bool QueueLess(const int i, const int j) const;
    void QueueSwap(const int i, const int j);
    void QueueUp(int j);
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

// points[p]
// triangles[e] = p
// halfedges[e] = e
// candidates[t]
// errors[t]
// queueIndexes[t] = q
// queue[q] = t
