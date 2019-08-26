#include <chrono>
#include <functional>
#include <iostream>

#include "cmdline.h"
#include "heightmap.h"
#include "pool.h"
#include "stl.h"
#include "triangulator.h"

/*

TODO / NOTES
============
- thread pool doesn't help much usually - get rid of it?
- add a solid base
- export a normal map?
- automatically compute some z scale?
- better error handling, especially for file I/O
- better overflow handling - what's the largest supported heightmap?
- pre-triangulation filters? e.g. gaussian blur
- OpenCL rasterization?
- mesh validation?

*/

int main(int argc, char **argv) {
    const auto startTime = std::chrono::steady_clock::now();

    // parse command line arguments
    cmdline::parser p;

    // long name, short name (or '\0'), description, mandatory (vs optional),
    // default value, constraint
    p.add<float>("zscale", 'z', "z scale relative to x & y", true);
    p.add<float>("zexagg", 'x', "z exaggeration", false, 1);
    p.add<float>("error", 'e', "maximum triangulation error", false, 0.001);
    p.add<int>("triangles", 't', "maximum number of triangles", false, 0);
    p.add<int>("points", 'p', "maximum number of vertices", false, 0);
    p.add("quiet", 'q', "suppress console output");
    p.footer("infile outfile.stl");
    p.parse_check(argc, argv);

    if (p.rest().size() != 2) {
        std::cerr << "infile and outfile required" << std::endl << p.usage();
        std::exit(1);
    }

    // extract command line arguments
    const std::string inFile = p.rest()[0];
    const std::string outFile = p.rest()[1];
    const float zScale = p.get<float>("zscale");
    const float zExaggeration = p.get<float>("zexagg");
    const float maxError = p.get<float>("error");
    const int maxTriangles = p.get<int>("triangles");
    const int maxPoints = p.get<int>("points");
    const bool quiet = p.exist("quiet");

    // helper function to display elapsed time of each step
    const auto timed = [quiet](const std::string &message)
        -> std::function<void()>
    {
        if (quiet) {
            return [](){};
        }
        printf("%s... ", message.c_str());
        fflush(stdout);
        const auto startTime = std::chrono::steady_clock::now();
        return [message, startTime]() {
            const std::chrono::duration<double> elapsed =
                std::chrono::steady_clock::now() - startTime;
            printf("%gs\n", elapsed.count());
        };
    };

    // load heightmap
    auto done = timed("loading heightmap");
    const auto hm = std::make_shared<Heightmap>(inFile);
    done();

    const int w = hm->Width();
    const int h = hm->Height();
    if (w * h == 0) {
        std::cerr
            << "invalid heightmap file (try png, jpg, etc.)" << std::endl
            << p.usage();
        std::exit(1);
    }

    // display statistics
    if (!quiet) {
        printf("  %d x %d = %d pixels\n", w, h, w * h);
    }

    // create thread pool
    const auto pool = std::make_shared<ThreadPool>();

    // construct triangulator
    done = timed("triangulating");
    Triangulator tri(hm, pool);

    // helper function to check if triangulation is complete
    const auto isDone = [&tri, maxError, maxTriangles, maxPoints]() {
        if (tri.Error() <= maxError) {
            return true;
        }
        if (maxTriangles > 0 && tri.NumTriangles() >= maxTriangles) {
            return true;
        }
        if (maxPoints > 0 && tri.NumPoints() >= maxPoints) {
            return true;
        }
        return false;
    };

    // triangulate
    while (!isDone()) {
        tri.Step();
    }

    // extract triangulation
    const auto points = tri.Points(zScale * zExaggeration);
    const auto triangles = tri.Triangles();
    done();

    // display statistics
    if (!quiet) {
        printf("  error = %g\n", tri.Error());
        printf("  points = %ld\n", points.size());
        printf("  triangles = %ld\n", triangles.size());
    }

    // write output file
    done = timed("writing output");
    SaveBinarySTL(outFile, points, triangles);
    done();

    // show total elapsed time
    if (!quiet) {
        const std::chrono::duration<double> elapsed =
            std::chrono::steady_clock::now() - startTime;
        printf("%gs\n", elapsed.count());
    }

    return 0;
}
