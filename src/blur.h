#pragma once

#include <vector>

std::vector<float> GaussianBlur(
    const std::vector<float> &data,
    const int w, const int h, const int r);
