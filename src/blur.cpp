#include "blur.h"

#include <cmath>

// see: http://blog.ivank.net/fastest-gaussian-blur.html

namespace {

std::vector<int> BoxesForGaussian(const float sigma, const int n) {
    const float wIdeal = std::sqrt((12 * sigma * sigma / n) + 1);
    int wl = wIdeal;
    if (wl % 2 == 0) {
        wl--;
    }
    const int wu = wl + 2;

    const float mIdeal =
        (12 * sigma * sigma - n * wl * wl - 4 * n * wl - 3 * n) /
        (-4 * wl - 4);
    const int m = std::round(mIdeal);

    std::vector<int> sizes;
    for (int i = 0; i < n; i++) {
        sizes.push_back(i < m ? wl : wu);
    }
    return sizes;
}

void BoxBlurH(
    std::vector<float> &scl,
    std::vector<float> &tcl,
    const int w, const int h, const int r)
{
    const float iarr = 1.f / (r + r + 1);
    for (int i = 0; i < h; i++) {
        int ti = i * w;
        int li = ti;
        int ri = ti + r;
        float fv = scl[ti];
        float lv = scl[ti + w - 1];
        float val = (r + 1) * fv;
        for (int j = 0; j < r; j++) {
            val += scl[ti + j];
        }
        for (int j = 0; j <= r; j++) {
            val += scl[ri] - fv;
            tcl[ti] = val * iarr;
            ri++;
            ti++;
        }
        for (int j = r + 1; j < w - r; j++) {
            val += scl[ri] - scl[li];
            tcl[ti] = val * iarr;
            li++;
            ri++;
            ti++;
        }
        for (int j = w - r; j < w; j++) {
            val += lv - scl[li];
            tcl[ti] = val * iarr;
            li++;
            ti++;
        }
    }
}

void BoxBlurV(
    std::vector<float> &scl,
    std::vector<float> &tcl,
    const int w, const int h, const int r)
{
    const float iarr = 1.f / (r + r + 1);
    for (int i = 0; i < w; i++) {
        int ti = i;
        int li = ti;
        int ri = ti + r * w;
        float fv = scl[ti];
        float lv = scl[ti + w * (h - 1)];
        float val = (r + 1) * fv;
        for (int j = 0; j < r; j++) {
            val += scl[ti + j * w];
        }
        for (int j = 0; j <= r; j++) {
            val += scl[ri] - fv;
            tcl[ti] = val * iarr;
            ri += w;
            ti += w;
        }
        for (int j = r + 1; j < h - r; j++) {
            val += scl[ri] - scl[li];
            tcl[ti] = val * iarr;
            li += w;
            ri += w;
            ti += w;
        }
        for (int j = h - r; j < h; j++) {
            val += lv - scl[li];
            tcl[ti] = val * iarr;
            li += w;
            ti += w;
        }
    }
}


void BoxBlur(
    std::vector<float> &scl,
    std::vector<float> &tcl,
    const int w, const int h, const int r)
{
    tcl.assign(scl.begin(), scl.end());
    BoxBlurH(tcl, scl, w, h, r);
    BoxBlurV(scl, tcl, w, h, r);
}

}

std::vector<float> GaussianBlur(
    const std::vector<float> &data,
    const int w, const int h, const int r)
{
    std::vector<float> scl = data;
    std::vector<float> tcl(data.size());
    const std::vector<int> boxes = BoxesForGaussian(r, 3);
    BoxBlur(scl, tcl, w, h, (boxes[0] - 1) / 2);
    BoxBlur(tcl, scl, w, h, (boxes[1] - 1) / 2);
    BoxBlur(scl, tcl, w, h, (boxes[2] - 1) / 2);
    return tcl;
}
