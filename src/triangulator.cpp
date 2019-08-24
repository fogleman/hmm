#include "triangulator.h"

#include "geometry.h"

Triangulator::Triangulator(const std::shared_ptr<Heightmap> &heightmap) :
    m_Heightmap(heightmap)
{
    // add initial two triangles
    const int x0 = 0;
    const int y0 = 0;
    const int x1 = m_Heightmap->Width() - 1;
    const int y1 = m_Heightmap->Height() - 1;
    const int p0 = AddPoint(glm::ivec2(x0, y0));
    const int p1 = AddPoint(glm::ivec2(x1, y0));
    const int p2 = AddPoint(glm::ivec2(x0, y1));
    const int p3 = AddPoint(glm::ivec2(x1, y1));
    const int t0 = AddTriangle(p3, p0, p2, -1, -1, -1);
    AddTriangle(p0, p3, p1, t0 * 3, -1, -1);
}

float Triangulator::Error() const {
    return m_Errors[m_Queue[0]];
}

void Triangulator::Step() {
    const int t = QueuePop();

    // printf("%d %g\n", t, m_Errors[t]);

    const int e0 = t * 3 + 0;
    const int e1 = t * 3 + 1;
    const int e2 = t * 3 + 2;

    const int p0 = m_Triangles[e0];
    const int p1 = m_Triangles[e1];
    const int p2 = m_Triangles[e2];

    const int h0 = m_Halfedges[e0];
    const int h1 = m_Halfedges[e1];
    const int h2 = m_Halfedges[e2];

    const glm::ivec2 a = m_Points[p0];
    const glm::ivec2 b = m_Points[p1];
    const glm::ivec2 c = m_Points[p2];
    const glm::ivec2 p = m_Candidates[t];

    const int pn = AddPoint(p);

    // const auto handleCollinear = [this](
    //     const glm::ivec2 a, const glm::ivec2 b, const glm::ivec2 c,
    //     const glm::ivec2 p)
    // {
    //     const glm::ivec4 e0 = MakeEdge(a, b);
    //     const glm::ivec4 e1 = MakeEdge(b, a);
    //     const auto s = m_Halfedges[e1];
    //     m_Halfedges.erase(e0);
    //     m_Halfedges.erase(e1);
    //     if (!s) {
    //         AddTriangle(b, c, p);
    //         AddTriangle(c, a, p);
    //         Legalize(b, c);
    //         Legalize(c, a);
    //         return;
    //     }
    //     m_Queue.Remove(s);
    //     const glm::ivec2 d = s->PointAfter(a);
    //     AddTriangle(b, c, p);
    //     AddTriangle(c, a, p);
    //     AddTriangle(a, d, p);
    //     AddTriangle(d, b, p);
    //     Legalize(b, c);
    //     Legalize(c, a);
    //     Legalize(a, d);
    //     Legalize(d, b);
    // };

    const auto handleCollinear = [this](const int pn, const int a) {
        const int b = m_Halfedges[a];

        if (b < 0) {
            const int a0 = a - a % 3;
            const int al = a0 + (a + 1) % 3;
            const int ar = a0 + (a + 2) % 3;
            const int p0 = m_Triangles[ar];
            const int pr = m_Triangles[a];
            const int pl = m_Triangles[al];
            const int hal = m_Halfedges[al];
            const int har = m_Halfedges[ar];
            const int t0 = AddTriangle(pn, p0, pr, -1, har, -1);
            const int t1 = AddTriangle(p0, pn, pl, t0 * 3, -1, hal);
            Legalize(t0 * 3 + 1);
            Legalize(t1 * 3 + 2);
            return;
        }

        const int a0 = a - a % 3;
        const int b0 = b - b % 3;

        const int al = a0 + (a + 1) % 3;
        const int ar = a0 + (a + 2) % 3;
        const int bl = b0 + (b + 2) % 3;
        const int br = b0 + (b + 1) % 3;

        const int p0 = m_Triangles[ar];
        const int pr = m_Triangles[a];
        const int pl = m_Triangles[al];
        const int p1 = m_Triangles[bl];

        const int hal = m_Halfedges[al];
        const int har = m_Halfedges[ar];
        const int hbl = m_Halfedges[bl];
        const int hbr = m_Halfedges[br];

        const int bt = b / 3;
        QueueRemove(bt);
        UnlinkTriangle(bt);

        const int t0 = AddTriangle(p0, pr, pn, har, -1, -1);
        const int t1 = AddTriangle(pr, p1, pn, hbr, -1, t0 * 3 + 1);
        const int t2 = AddTriangle(p1, pl, pn, hbl, -1, t1 * 3 + 1);
        const int t3 = AddTriangle(pl, p0, pn, hal, t0 * 3 + 2, t2 * 3 + 1);
        Legalize(t0 * 3);
        Legalize(t1 * 3);
        Legalize(t2 * 3);
        Legalize(t3 * 3);
    };

    if (Collinear(a, b, p)) {
        handleCollinear(pn, e0);
        // handleCollinear(a, b, c, p);
        UnlinkTriangle(t);
    } else if (Collinear(b, c, p)) {
        handleCollinear(pn, e1);
        // handleCollinear(b, c, a, p);
        UnlinkTriangle(t);
    } else if (Collinear(c, a, p)) {
        handleCollinear(pn, e2);
        // handleCollinear(c, a, b, p);
        UnlinkTriangle(t);
    } else {
        UnlinkTriangle(t);
        const int t0 = AddTriangle(p0, p1, pn, h0, -1, -1);
        const int t1 = AddTriangle(p1, p2, pn, h1, -1, t0 * 3 + 1);
        const int t2 = AddTriangle(p2, p0, pn, h2, t0 * 3 + 2, t1 * 3 + 1);
        Legalize(t0 * 3);
        Legalize(t1 * 3);
        Legalize(t2 * 3);
        // AddTriangle(a, b, p);
        // AddTriangle(b, c, p);
        // AddTriangle(c, a, p);
        // Legalize(a, b);
        // Legalize(b, c);
        // Legalize(c, a);
    }
}

int Triangulator::AddPoint(const glm::ivec2 point) {
    const int i = m_Points.size();
    m_Points.push_back(point);
    return i;
}

int Triangulator::AddTriangle(
    const int a, const int b, const int c,
    const int ab, const int bc, const int ca)
{
    // rasterize triangle to find maximum pixel error
    const auto pair = m_Heightmap->FindCandidate(
        m_Points[a], m_Points[b], m_Points[c]);
    // new triangle index
    const int t = m_Candidates.size();
    // new halfedge index
    const int e = m_Triangles.size();
    // add triangle vertices
    m_Triangles.push_back(a);
    m_Triangles.push_back(b);
    m_Triangles.push_back(c);
    // add triangle halfedges
    m_Halfedges.push_back(ab);
    m_Halfedges.push_back(bc);
    m_Halfedges.push_back(ca);
    // link neighboring halfedges
    if (ab >= 0) m_Halfedges[ab] = e + 0;
    if (bc >= 0) m_Halfedges[bc] = e + 1;
    if (ca >= 0) m_Halfedges[ca] = e + 2;
    // add triangle metadata
    m_Candidates.push_back(pair.first);
    m_Errors.push_back(pair.second);
    m_QueueIndexes.push_back(-1);
    // add triangle to priority queue
    QueuePush(t);
    // return triangle index
    return t;
}

void Triangulator::UnlinkTriangle(const int t) {
    m_Halfedges[t * 3 + 0] = -1;
    m_Halfedges[t * 3 + 1] = -1;
    m_Halfedges[t * 3 + 2] = -1;
}

void Triangulator::Link(const int a, const int b) {
    m_Halfedges[a] = b;
    if (b >= 0) {
        m_Halfedges[b] = a;
    }
}

void Triangulator::Legalize(const int a) {
    // if the pair of triangles doesn't satisfy the Delaunay condition
    // (p1 is inside the circumcircle of [p0, pl, pr]), flip them,
    // then do the same check/flip recursively for the new pair of triangles
    //
    //           pl                    pl
    //          /||\                  /  \
    //       al/ || \bl            al/    \a
    //        /  ||  \              /      \
    //       /  a||b  \    flip    /___ar___\
    //     p0\   ||   /p1   =>   p0\---bl---/p1
    //        \  ||  /              \      /
    //       ar\ || /br             b\    /br
    //          \||/                  \  /
    //           pr                    pr

    const int b = m_Halfedges[a];

    if (b < 0) {
        return;
    }

    const int a0 = a - a % 3;
    const int b0 = b - b % 3;

    const int al = a0 + (a + 1) % 3;
    const int ar = a0 + (a + 2) % 3;
    const int bl = b0 + (b + 2) % 3;
    const int br = b0 + (b + 1) % 3;

    const int p0 = m_Triangles[ar];
    const int pr = m_Triangles[a];
    const int pl = m_Triangles[al];
    const int p1 = m_Triangles[bl];

    if (!InCircle(m_Points[p0], m_Points[pr], m_Points[pl], m_Points[p1])) {
        return;
    }

    const int hal = m_Halfedges[al];
    const int har = m_Halfedges[ar];
    const int hbl = m_Halfedges[bl];
    const int hbr = m_Halfedges[br];

    const int at = a / 3;
    const int bt = b / 3;
    QueueRemove(at);
    QueueRemove(bt);
    UnlinkTriangle(at);
    UnlinkTriangle(bt);

    const int t0 = AddTriangle(p0, p1, pl, -1, hbl, hal);
    const int t1 = AddTriangle(p1, p0, pr, t0 * 3, har, hbr);

    Legalize(t0 * 3 + 1);
    Legalize(t1 * 3 + 2);
}

// priority queue functions

void Triangulator::QueuePush(const int t) {
    const int i = m_Queue.size();
    m_QueueIndexes[t] = i;
    m_Queue.push_back(t);
    QueueUp(i);
}

int Triangulator::QueuePop() {
    const int n = m_Queue.size() - 1;
    QueueSwap(0, n);
    QueueDown(0, n);
    return QueuePopBack();
}

int Triangulator::QueuePopBack() {
    const int t = m_Queue.back();
    m_Queue.pop_back();
    m_QueueIndexes[t] = -1;
    return t;
}

void Triangulator::QueueRemove(const int t) {
    const int i = m_QueueIndexes[t];
    const int n = m_Queue.size() - 1;
    if (n != i) {
        QueueSwap(i, n);
        if (!QueueDown(i, n)) {
            QueueUp(i);
        }
    }
    QueuePopBack();
}

bool Triangulator::QueueLess(const int i, const int j) const {
    return -m_Errors[m_Queue[i]] < -m_Errors[m_Queue[j]];
}

void Triangulator::QueueSwap(const int i, const int j) {
    const int pi = m_Queue[i];
    const int pj = m_Queue[j];
    m_Queue[i] = pj;
    m_Queue[j] = pi;
    m_QueueIndexes[pi] = j;
    m_QueueIndexes[pj] = i;
}

void Triangulator::QueueUp(int j) {
    while (1) {
        int i = (j - 1) / 2;
        if (i == j || !QueueLess(j, i)) {
            break;
        }
        QueueSwap(i, j);
        j = i;
    }
}

bool Triangulator::QueueDown(const int i0, const int n) {
    int i = i0;
    while (1) {
        const int j1 = 2 * i + 1;
        if (j1 >= n || j1 < 0) {
            break;
        }
        const int j2 = j1 + 1;
        int j = j1;
        if (j2 < n && QueueLess(j2, j1)) {
            j = j2;
        }
        if (!QueueLess(j, i)) {
            break;
        }
        QueueSwap(i, j);
        i = j;
    }
    return i > i0;
}

// debug output

void Triangulator::Dump() const {
    printf("%ld\n", m_Queue.size());
    return;

    printf("\n");
    printf("m_Points:\n");
    for (int i = 0; i < m_Points.size(); i++) {
        printf("%d: %d, %d\n", i, m_Points[i].x, m_Points[i].y);
    }
    printf("\n");

    printf("m_Triangles:\n");
    for (int i = 0; i < m_Triangles.size(); i++) {
        printf("%d: %d\n", i, m_Triangles[i]);
    }
    printf("\n");

    printf("m_Halfedges:\n");
    for (int i = 0; i < m_Halfedges.size(); i++) {
        printf("%d: %d\n", i, m_Halfedges[i]);
    }
    printf("\n");

    printf("m_Candidates:\n");
    for (int i = 0; i < m_Candidates.size(); i++) {
        printf("%d: %d, %d\n", i, m_Candidates[i].x, m_Candidates[i].y);
    }
    printf("\n");

    printf("m_Errors:\n");
    for (int i = 0; i < m_Errors.size(); i++) {
        printf("%d: %g\n", i, m_Errors[i]);
    }
    printf("\n");

    printf("m_QueueIndexes:\n");
    for (int i = 0; i < m_QueueIndexes.size(); i++) {
        printf("%d: %d\n", i, m_QueueIndexes[i]);
    }
    printf("\n");

    printf("m_Queue:\n");
    for (int i = 0; i < m_Queue.size(); i++) {
        printf("%d: %d\n", i, m_Queue[i]);
    }
    printf("\n");
}
