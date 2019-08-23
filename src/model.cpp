#include "model.h"

#include "geometry.h"

namespace {

glm::ivec4 MakeEdge(const glm::ivec2 a, const glm::ivec2 b) {
    return glm::ivec4(a.x, a.y, b.x, b.y);
}

}

Model::Model(const std::shared_ptr<Heightmap> &heightmap) :
    m_Heightmap(heightmap)
{
    // add initial two triangles
    const int x1 = m_Heightmap->Width() - 1;
    const int y1 = m_Heightmap->Height() - 1;
    const glm::ivec2 p00(0, 0);
    const glm::ivec2 p01(0, y1);
    const glm::ivec2 p10(x1, 0);
    const glm::ivec2 p11(x1, y1);
    AddTriangle(p00, p01, p11);
    AddTriangle(p00, p11, p10);
}

const std::vector<std::shared_ptr<Triangle>> &Model::Triangles() const {
    return m_Queue.Triangles();
}

float Model::Error() const {
    return m_Queue.Head()->Error();
}

void Model::Step() {
    const auto t = m_Queue.Pop();
    const glm::ivec2 a = t->A();
    const glm::ivec2 b = t->B();
    const glm::ivec2 c = t->C();
    const glm::ivec2 p = t->Candidate();

    const auto handleCollinear = [this](
        const glm::ivec2 a, const glm::ivec2 b, const glm::ivec2 c,
        const glm::ivec2 p)
    {
        const glm::ivec4 e0 = MakeEdge(a, b);
        const glm::ivec4 e1 = MakeEdge(b, a);
        const auto s = m_Halfedges[e1];
        m_Halfedges.erase(e0);
        m_Halfedges.erase(e1);
        if (!s) {
            AddTriangle(b, c, p);
            AddTriangle(c, a, p);
            Legalize(b, c);
            Legalize(c, a);
            return;
        }
        m_Queue.Remove(s);
        const glm::ivec2 d = s->PointAfter(a);
        AddTriangle(b, c, p);
        AddTriangle(c, a, p);
        AddTriangle(a, d, p);
        AddTriangle(d, b, p);
        Legalize(b, c);
        Legalize(c, a);
        Legalize(a, d);
        Legalize(d, b);
    };

    if (Collinear(a, b, p)) {
        handleCollinear(a, b, c, p);
    } else if (Collinear(b, c, p)) {
        handleCollinear(b, c, a, p);
    } else if (Collinear(c, a, p)) {
        handleCollinear(c, a, b, p);
    } else {
        AddTriangle(a, b, p);
        AddTriangle(b, c, p);
        AddTriangle(c, a, p);
        Legalize(a, b);
        Legalize(b, c);
        Legalize(c, a);
    }
}

void Model::AddTriangle(
    const glm::ivec2 a, 
    const glm::ivec2 b, 
    const glm::ivec2 c)
{
    // TODO: possible unnecessary rasterization during legalize
    const auto pair = m_Heightmap->FindCandidate(a, b, c);
    const auto t = std::make_shared<Triangle>(a, b, c, pair.first, pair.second);
    m_Halfedges[MakeEdge(a, b)] = t;
    m_Halfedges[MakeEdge(b, c)] = t;
    m_Halfedges[MakeEdge(c, a)] = t;
    m_Queue.Push(t);
}

void Model::Legalize(const glm::ivec2 p0, const glm::ivec2 p1) {
    const glm::ivec4 e0 = MakeEdge(p0, p1);
    const glm::ivec4 e1 = MakeEdge(p1, p0);
    // lookup triangle from edge
    const auto t = m_Halfedges[e0];
    // lookup neighboring triangle
    const auto s = m_Halfedges[e1];
    // nothing to do if no neighbor
    if (!s) {
        return;
    }
    // get other triangle points
    const glm::ivec2 p2 = t->PointAfter(p1);
    const glm::ivec2 p3 = s->PointAfter(p0);
    // check delaunay condition
    if (!InCircle(p0, p1, p2, p3)) {
        return;
    }
    // remove triangles from priority queue
    m_Queue.Remove(t);
    m_Queue.Remove(s);
    // remove edges that will no longer exist
    m_Halfedges.erase(e0);
    m_Halfedges.erase(e1);
    // add new triangles
    AddTriangle(p1, p2, p3);
    AddTriangle(p0, p3, p2);
    // recursively check next edges
    Legalize(p3, p1);
    Legalize(p0, p3);
}
