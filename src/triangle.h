#pragma once

#include <glm/glm.hpp>

class Triangle {
public:
    Triangle(
        const glm::ivec2 a, const glm::ivec2 b, const glm::ivec2 c,
        const glm::ivec2 candidate, const float error) :
        m_A(a), m_B(b), m_C(c),
        m_Candidate(candidate), m_Error(error), m_QueueIndex(-1) {}

    const glm::ivec2 A() const {
        return m_A;
    }

    const glm::ivec2 B() const {
        return m_B;
    }

    const glm::ivec2 C() const {
        return m_C;
    }

    const glm::ivec2 PointAfter(const glm::ivec2 p) const {
        if (p == m_A) {
            return m_B;
        } else if (p == m_B) {
            return m_C;
        } else {
            return m_A;
        }
    }

    const glm::ivec2 Candidate() const {
        return m_Candidate;
    }

    float Error() const {
        return m_Error;
    }

    int QueueIndex() const {
        return m_QueueIndex;
    }

    void SetQueueIndex(const int index) {
        m_QueueIndex = index;
    }

private:
    glm::ivec2 m_A;
    glm::ivec2 m_B;
    glm::ivec2 m_C;
    glm::ivec2 m_Candidate;
    float m_Error;
    int m_QueueIndex;
};
