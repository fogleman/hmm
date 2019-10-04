#include <chrono>
#include <functional>
#include <iostream>
#include <string>

#include "base.h"
#include "cmdline.h"
#include "heightmap.h"
#include "stl.h"
#include "triangulator.h"

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
    p.add("level", '\0', "auto level input to full grayscale range");
    p.add("invert", '\0', "invert heightmap");
    p.add<int>("blur", '\0', "gaussian blur sigma", false, 0);
    p.add<float>("gamma", '\0', "gamma curve exponent", false, 0);
    p.add<int>("border-size", '\0', "border size in pixels", false, 0);
    p.add<float>("border-height", '\0', "border z height", false, 1);
    p.add<std::string>("normal-map", '\0', "path to write normal map png", false, "");
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
    const float baseHeight = p.get<float>("base");
    const bool level = p.exist("level");
    const bool invert = p.exist("invert");
    const int blurSigma = p.get<int>("blur");
    const float gamma = p.get<float>("gamma");
    const int borderSize = p.get<int>("border-size");
    const float borderHeight = p.get<float>("border-height");
    const std::string normalmapPath = p.get<std::string>("normal-map");
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

    int w = hm->Width();
    int h = hm->Height();
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

    // auto level heightmap
    if (level) {
        hm->AutoLevel();
    }

    // invert heightmap
    if (invert) {
        hm->Invert();
    }

    // blur heightmap
    if (blurSigma > 0) {
        done = timed("blurring heightmap");
        hm->GaussianBlur(blurSigma);
        done();
    }

    // apply gamma curve
    if (gamma > 0) {
        hm->GammaCurve(gamma);
    }

    // add border
    if (borderSize > 0) {
        hm->AddBorder(borderSize, borderHeight);
    }

    // get updated size
    w = hm->Width();
    h = hm->Height();

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
    SaveBinarySTL(outFile, points, triangles);
    done();

    // compute normal map
    if (!normalmapPath.empty()) {
        done = timed("computing normal map");
        hm->SaveNormalmap(normalmapPath, zScale * zExaggeration);
        done();
    }

    // show total elapsed time
    if (!quiet) {
        const std::chrono::duration<double> elapsed =
            std::chrono::steady_clock::now() - startTime;
        printf("%gs\n", elapsed.count());
    }

    return 0;
}
