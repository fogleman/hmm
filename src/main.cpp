#include <glm/glm.hpp>
#include <iostream>

#include "heightmap.h"
#include "model.h"

int main(int argc, char **argv) {
    const auto hm = std::make_shared<Heightmap>(argv[1]);
    // Heightmap hm(argv[1]);

    const int w = hm->Width();
    const int h = hm->Height();

    std::cout << w << "x" << h << std::endl;
    std::cout << hm->At(0, 0) << std::endl;

    Model model(hm);
    while (model.Error() > 0.001) {
        model.Step();
    }
    
    // const glm::ivec2 p00(0, 0);
    // const glm::ivec2 p01(0, h - 1);
    // const glm::ivec2 p10(w - 1, 0);
    // const glm::ivec2 p11(w - 1, h - 1);

    // for (int i = 0; i < 1000; i++) {
    //     const auto c = hm->FindCandidate(p00, p01, p10);
    //     if (i == 0) {
    //         std::cout << c.first.x << ", " << c.first.y << " " << c.second << std::endl;
    //     }
    // }

    return 0;
}
