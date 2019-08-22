#include <iostream>

#include "heightmap.h"

int main(int argc, char **argv) {
    Heightmap hm(argv[1]);
    std::cout << hm.Width() << "x" << hm.Height() << std::endl;
    std::cout << hm.At(0, 0) << std::endl;
    return 0;
}
