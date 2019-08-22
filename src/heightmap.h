#include <string>
#include <vector>

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

private:
    int m_Width;
    int m_Height;
    std::vector<uint16_t> m_Data;
};
