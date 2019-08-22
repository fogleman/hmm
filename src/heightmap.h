#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "candidate.h"

class Heightmap {
public:
    Heightmap(const std::string &path);

    int Width() const {
        return m_Width;
    }

    int Height() const {
        return m_Height;
    }

    uint16_t At(const int x, const int y) const {
        return m_Data[y * m_Width + x];
    }

    uint16_t At(const glm::ivec2 &p) const {
        return m_Data[p.y * m_Width + p.x];
    }

    Candidate FindCandidate(
        const glm::ivec2 &p0,
        const glm::ivec2 &p1,
        const glm::ivec2 &p2) const;

private:
    int m_Width;
    int m_Height;
    std::vector<uint16_t> m_Data;
};
