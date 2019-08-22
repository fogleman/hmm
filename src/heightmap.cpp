#include "heightmap.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Heightmap::Heightmap(const std::string &path) :
    m_Width(0),
    m_Height(0)
{
    int w, h, n;
    uint16_t *data = stbi_load_16(path.c_str(), &w, &h, &n, 1);
    if (data) {
        m_Width = w;
        m_Height = h;
        m_Data.assign(data, data + w * h);
        free(data);
    }
}

Candidate Heightmap::FindCandidate(
    const glm::ivec2 &p0, const glm::ivec2 &p1, const glm::ivec2 &p2) const
{
    Candidate candidate;

    const auto edge = [](const glm::ivec2 &a, const glm::ivec2 &b, const glm::ivec2 &c) {
        return (b.x - c.x) * (a.y - c.y) - (b.y - c.y) * (a.x - c.x);
    };

    // z values at vertices
    const uint16_t z0 = At(p0);
    const uint16_t z1 = At(p1);
    const uint16_t z2 = At(p2);

    // bounding box
    const glm::ivec2 min = glm::min(glm::min(p0, p1), p2);
    const glm::ivec2 max = glm::max(glm::max(p0, p1), p2);

    // forward differencing variables
    int w00 = edge(p1, p2, min);
    int w01 = edge(p2, p0, min);
    int w02 = edge(p0, p1, min);
    const int a01 = p1.y - p0.y;
    const int b01 = p0.x - p1.x;
    const int a12 = p2.y - p1.y;
    const int b12 = p1.x - p2.x;
    const int a20 = p0.y - p2.y;
    const int b20 = p2.x - p0.x;

    // reciprocals
    const float ra = 1.f / edge(p0, p1, p2);
    // const float ra12 = 1.f / a12;
    // const float ra20 = 1.f / a20;
    // const float ra01 = 1.f / a01;

    // iterate over pixels in bounding box
    for (int y = min.y; y <= max.y; y++) {
        int d = 0;
        const int d0 = -w00 / a12;
        const int d1 = -w01 / a20;
        const int d2 = -w02 / a01;
        if (w00 < 0 && d0 > d) {
            d = d0;
        }
        if (w01 < 0 && d1 > d) {
            d = d1;
        }
        if (w02 < 0 && d2 > d) {
            d = d2;
        }

        int w0 = w00 + a12 * d;
        int w1 = w01 + a20 * d;
        int w2 = w02 + a01 * d;
        bool wasInside = false;
        for (int x = min.x + d; x <= max.x; x++) {
            const float b0 = w0 * ra;
            const float b1 = w1 * ra;
            const float b2 = w2 * ra;
            w0 += a12;
            w1 += a20;
            w2 += a01;

            // check if inside triangle
            if (b0 < 0 || b1 < 0 || b2 < 0) {
                if (wasInside) {
                    break;
                }
                continue;
            }
            wasInside = true;

            // compute z using barycentric coordinates
            const float z = z0 * b0 + z1 * b1 + z2 * b2;
            const float dz = std::abs(z - At(x, y));
            if (dz > candidate.Error()) {
                candidate = Candidate(glm::ivec2(x, y), dz);
            }
        }
        w00 += b12;
        w01 += b20;
        w02 += b01;
    }

    return candidate;
}
