#include "stl.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <fstream>
#include <glm/gtx/normal.hpp>
#include <cstring>

void SaveBinarySTL(
    const std::string &path,
    const std::vector<glm::vec3> &points,
    const std::vector<glm::ivec3> &triangles)
{
    // TODO: properly handle endian-ness

    const uint64_t numBytes = uint64_t(triangles.size()) * 50 + 84;
    char *dst = (char *)calloc(numBytes, 1);

    const uint32_t count = triangles.size();
    memcpy(dst + 80, &count, 4);

    for (uint32_t i = 0; i < triangles.size(); i++) {
        const glm::ivec3 t = triangles[i];
        const glm::vec3 p0 = points[t.x];
        const glm::vec3 p1 = points[t.y];
        const glm::vec3 p2 = points[t.z];
        const glm::vec3 normal = glm::triangleNormal(p0, p1, p2);
        const uint64_t idx = 84 + i * 50;
        memcpy(dst + idx, &normal, 12);
        memcpy(dst + idx + 12, &p0, 12);
        memcpy(dst + idx + 24, &p1, 12);
        memcpy(dst + idx + 36, &p2, 12);
    }

    std::fstream file(path, std::ios::out | std::ios::binary);
    file.write(dst, numBytes);
    file.close();

    free(dst);
}
