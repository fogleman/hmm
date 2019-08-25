#include <glm/glm.hpp>
#include <iostream>

#include "heightmap.h"
#include "stl.h"
#include "triangulator.h"

int main(int argc, char **argv) {
    const auto hm = std::make_shared<Heightmap>(argv[1]);

    const int w = hm->Width();
    const int h = hm->Height();

    std::cout << w << "x" << h << std::endl;
    std::cout << hm->At(0, 0) << std::endl;

    Triangulator tri(hm);
    while (tri.Error() > 0.001) {
        tri.Step();
    }
    tri.Dump();

    SaveBinarySTL("out.stl", tri.Points(), tri.Triangles());

    return 0;
}
