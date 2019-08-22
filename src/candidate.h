#include <glm/glm.hpp>

class Candidate {
public:
    Candidate(const glm::ivec2 &point, const double error);
private:
    glm::ivec2 m_Point;
    double m_Error;
};
