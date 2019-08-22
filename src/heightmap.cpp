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
