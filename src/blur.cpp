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
    std::vector<float> &src,
    std::vector<float> &dst,
    const int w, const int h, const int r)
{
    const float m = 1.f / (r + r + 1);
    for (int i = 0; i < h; i++) {
        int ti = i * w;
        int li = ti;
        int ri = ti + r;
        float fv = src[ti];
        float lv = src[ti + w - 1];
        float val = (r + 1) * fv;
        for (int j = 0; j < r; j++) {
            val += src[ti + j];
        }
        for (int j = 0; j <= r; j++) {
            val += src[ri] - fv;
            dst[ti] = val * m;
            ri++;
            ti++;
        }
        for (int j = r + 1; j < w - r; j++) {
            val += src[ri] - src[li];
            dst[ti] = val * m;
            li++;
            ri++;
            ti++;
        }
        for (int j = w - r; j < w; j++) {
            val += lv - src[li];
            dst[ti] = val * m;
            li++;
            ti++;
        }
    }
}

void BoxBlurV(
    std::vector<float> &src,
    std::vector<float> &dst,
    const int w, const int h, const int r)
{
    const float m = 1.f / (r + r + 1);
    for (int i = 0; i < w; i++) {
        int ti = i;
        int li = ti;
        int ri = ti + r * w;
        float fv = src[ti];
        float lv = src[ti + w * (h - 1)];
        float val = (r + 1) * fv;
        for (int j = 0; j < r; j++) {
            val += src[ti + j * w];
        }
        for (int j = 0; j <= r; j++) {
            val += src[ri] - fv;
            dst[ti] = val * m;
            ri += w;
            ti += w;
        }
        for (int j = r + 1; j < h - r; j++) {
            val += src[ri] - src[li];
            dst[ti] = val * m;
            li += w;
            ri += w;
            ti += w;
        }
        for (int j = h - r; j < h; j++) {
            val += lv - src[li];
            dst[ti] = val * m;
            li += w;
            ti += w;
        }
    }
}


void BoxBlur(
    std::vector<float> &src,
    std::vector<float> &dst,
    const int w, const int h, const int r)
{
    dst.assign(src.begin(), src.end());
    BoxBlurH(dst, src, w, h, r);
    BoxBlurV(src, dst, w, h, r);
}

}

std::vector<float> GaussianBlur(
    const std::vector<float> &data,
    const int w, const int h, const int r)
{
    std::vector<float> src = data;
    std::vector<float> dst(data.size());
    const std::vector<int> boxes = BoxesForGaussian(r, 3);
    BoxBlur(src, dst, w, h, (boxes[0] - 1) / 2);
    BoxBlur(dst, src, w, h, (boxes[1] - 1) / 2);
    BoxBlur(src, dst, w, h, (boxes[2] - 1) / 2);
    return dst;
}
