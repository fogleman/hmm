#include <chrono>
#include <functional>
#include <iostream>

#include "base.h"
#include "cmdline.h"
#include "heightmap.h"
#include "stl.h"
#include "obj.h"
#include "triangulator.h"

static std::string GetFileExt(const std::string& fn)
{
    size_t pos = fn.find_last_of('.');
    if (pos != std::string::npos)
        return fn.substr(pos);
    return "";
}

static bool StringEquals(const std::string& l, const std::string& r)
{
    return l.size() == r.size()
        && std::equal(l.cbegin(), l.cend(), r.cbegin(),
            [](std::string::value_type l1, std::string::value_type r1)
    {
        return toupper(l1) == toupper(r1);
    });
}

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
    p.add<float>("base", 'b', "solid base height", false, 0);
    p.add("quiet", 'q', "suppress console output");
    p.footer("infile outfile.{stl,obj}");
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
    const float baseHeight = p.get<float>("base");
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

    // triangulate
    done = timed("triangulating");
    Triangulator tri(hm);
    tri.Run(maxError, maxTriangles, maxPoints);
    auto points = tri.Points(zScale * zExaggeration);
    auto triangles = tri.Triangles();
    done();

    // add base
    if (baseHeight > 0) {
        done = timed("adding solid base");
        const float z = -baseHeight * zScale * zExaggeration;
        AddBase(points, triangles, w, h, z);
        done();
    }

    // display statistics
    if (!quiet) {
        const int naiveTriangleCount = (w - 1) * (h - 1) * 2;
        printf("  error = %g\n", tri.Error());
        printf("  points = %ld\n", points.size());
        printf("  triangles = %ld\n", triangles.size());
        printf("  vs. naive = %g%%\n", 100.f * triangles.size() / naiveTriangleCount);
    }

    // write output file
    done = timed("writing output");
    std::string ext = GetFileExt(outFile);
    if (StringEquals(ext, ".obj"))
        SaveOBJ(outFile, points, triangles);
    else
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
