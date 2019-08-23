#include "geometry.h"

bool Collinear(const glm::ivec2 p0, const glm::ivec2 p1, const glm::ivec2 p2) {
    return (p1.y-p0.y)*(p2.x-p1.x) == (p2.y-p1.y)*(p1.x-p0.x);
}

bool InCircle(
    const glm::ivec2 a, const glm::ivec2 b, const glm::ivec2 c,
    const glm::ivec2 p)
{
    const int64_t dx = a.x - p.x;
    const int64_t dy = a.y - p.y;
    const int64_t ex = b.x - p.x;
    const int64_t ey = b.y - p.y;
    const int64_t fx = c.x - p.x;
    const int64_t fy = c.y - p.y;
    const int64_t ap = dx * dx + dy * dy;
    const int64_t bp = ex * ex + ey * ey;
    const int64_t cp = fx * fx + fy * fy;
    return dx*(ey*cp-bp*fy)-dy*(ex*cp-bp*fx)+ap*(ex*fy-ey*fx) < 0;
}
