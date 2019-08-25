#include <iostream>

#include "heightmap.h"
#include "stl.h"
#include "triangulator.h"

int main(int argc, char **argv) {
    const auto hm = std::make_shared<Heightmap>(argv[1]);

    printf("%dx%d\n", hm->Width(), hm->Height());

    Triangulator tri(hm);
    while (tri.Error() > 0.001) {
    // while (tri.NumTriangles() < 1000000) {
        tri.Step();
    }

    const auto points = tri.Points(2000 * 2);
    const auto triangles = tri.Triangles();

    printf("error = %g\n", tri.Error());
    printf("%ld vertices\n", points.size());
    printf("%ld triangles\n", triangles.size());

    SaveBinarySTL("out.stl", points, triangles);

    return 0;
}
