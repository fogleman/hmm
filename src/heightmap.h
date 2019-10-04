#pragma once

#include <glm/glm.hpp>
#include <string>
#include <utility>
#include <vector>

class Heightmap {
public:
    Heightmap(const std::string &path);

    Heightmap(
        const int width,
        const int height,
        const std::vector<float> &data);

    int Width() const {
        return m_Width;
    }

    int Height() const {
        return m_Height;
    }

    float At(const int x, const int y) const {
        return m_Data[y * m_Width + x];
    }

    float At(const glm::ivec2 p) const {
        return m_Data[p.y * m_Width + p.x];
    }

    void AutoLevel();

    void Invert();

    void GammaCurve(const float gamma);

    void AddBorder(const int size, const float z);

    void GaussianBlur(const int r);

    std::vector<glm::vec3> Normalmap(const float zScale) const;

    void SaveNormalmap(const std::string &path, const float zScale) const;

    std::pair<glm::ivec2, float> FindCandidate(
        const glm::ivec2 p0,
        const glm::ivec2 p1,
        const glm::ivec2 p2) const;

private:
    int m_Width;
    int m_Height;
    std::vector<float> m_Data;
};
