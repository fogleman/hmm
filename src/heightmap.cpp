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
        const int n = w * h;
        const float m = 1.f / 65535.f;
        m_Data.resize(n);
        for (int i = 0; i < n; i++) {
            m_Data[i] = data[i] * m;
        }
        free(data);
    }
}

std::pair<glm::ivec2, float> Heightmap::FindCandidate(
    const glm::ivec2 p0,
    const glm::ivec2 p1,
    const glm::ivec2 p2) const
{
    const auto edge = [](
        const glm::ivec2 a, const glm::ivec2 b, const glm::ivec2 c)
    {
        return (b.x - c.x) * (a.y - c.y) - (b.y - c.y) * (a.x - c.x);
    };

    // triangle bounding box
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

    // pre-multiplied z values at vertices
    const float a = edge(p0, p1, p2);
    const float z0 = At(p0) / a;
    const float z1 = At(p1) / a;
    const float z2 = At(p2) / a;

    // iterate over pixels in bounding box
    float maxError = 0;
    glm::ivec2 maxPoint(0);
    for (int y = min.y; y <= max.y; y++) {
        // compute starting offset
        int dx = 0;
        if (w00 < 0 && a12 != 0) {
            dx = std::max(dx, -w00 / a12);
        }
        if (w01 < 0 && a20 != 0) {
            dx = std::max(dx, -w01 / a20);
        }
        if (w02 < 0 && a01 != 0) {
            dx = std::max(dx, -w02 / a01);
        }

        int w0 = w00 + a12 * dx;
        int w1 = w01 + a20 * dx;
        int w2 = w02 + a01 * dx;

        bool wasInside = false;

        for (int x = min.x + dx; x <= max.x; x++) {
            // check if inside triangle
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                wasInside = true;

                // compute z using barycentric coordinates
                const float z = z0 * w0 + z1 * w1 + z2 * w2;
                const float dz = std::abs(z - At(x, y));
                if (dz > maxError) {
                    maxError = dz;
                    maxPoint = glm::ivec2(x, y);
                }
            } else if (wasInside) {
                break;
            }

            w0 += a12;
            w1 += a20;
            w2 += a01;
        }

        w00 += b12;
        w01 += b20;
        w02 += b01;
    }

    return std::make_pair(maxPoint, maxError);
}
