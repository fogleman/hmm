#include <glm/glm.hpp>

class Candidate {
public:
    Candidate() :
        m_Point(0),
        m_Error(0) {}

    Candidate(const glm::ivec2 &point, const uint16_t error) :
        m_Point(point),
        m_Error(error) {}

    const glm::ivec2 &Point() const {
        return m_Point;
    }

    uint16_t Error() const {
        return m_Error;
    }

private:
    glm::ivec2 m_Point;
    uint16_t m_Error;
};
