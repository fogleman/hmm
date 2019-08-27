#include "obj.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <fstream>
#include <glm/gtx/normal.hpp>
#include <glm/ext.hpp>

void SaveOBJ(
    const std::string &path,
    const std::vector<glm::vec3> &points,
    const std::vector<glm::ivec3> &triangles)
{
    std::fstream file(path, std::fstream::out);

    file << "o Heightmap" << std::endl;

    file << "# " << points.size() << " Vertices" << std::endl;
    // Rotate by 270 degree around X axis
    glm::mat3 rotation = glm::rotate(4.712389f, glm::vec3(1.0f, 0.0f, 0.0f));
    for (const auto& point : points)
    {
        const glm::vec3 rotPoint = rotation * point;
        file << "v " << rotPoint.x << " " << rotPoint.y << " " << rotPoint.z << std::endl;
    }

    file << "# " << triangles.size() << " Normals" << std::endl;
    for (const auto& t : triangles)
    {
        const glm::vec3 p0 = points[t.x];
        const glm::vec3 p1 = points[t.y];
        const glm::vec3 p2 = points[t.z];
        const glm::vec3 normal = glm::triangleNormal(p0, p1, p2);
        file << "vn " << normal.x << " " << normal.y << " " << normal.z << std::endl;
    }

    file << "# " << triangles.size() << " Faces" << std::endl;
    unsigned normalIndex = 0;
    for (const auto& t : triangles)
    {
        // Indices in .obj files are 1-based
        ++normalIndex;
        file << "f " << t.x + 1 << "//" << normalIndex << " " << t.y + 1 << "//" << normalIndex << " " << t.z + 1 << "//" << normalIndex << std::endl;
    }

}
