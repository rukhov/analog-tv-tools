#pragma once

namespace atv {

template <typename T>
inline void normalize(T& v, T minv = 0, T maxv = 1)
{
    if (v < minv)
        v = minv;
    if (v > maxv)
        v = maxv;
}

struct YCbCr {
    // https://fourcc.org/indexyuv.htm

    YCbCr() : Y(16), Cb(128), Cr(128) {}
    YCbCr(uint8_t _Y, uint8_t _Cb, uint8_t _Cr) : Y(_Y), Cb(_Cb), Cr(_Cr) {}
    uint8_t Y;
    uint8_t Cb;
    uint8_t Cr;
};

struct RGB01 {
    // r, g, b - 0..1
    float r;
    float g;
    float b;

    RGB01& fix()
    {
        if (r < 0)
            r = 0;
        if (r > 1)
            r = 1;
        if (g < 0)
            g = 0;
        if (g > 1)
            g = 1;
        if (b < 0)
            b = 0;
        if (b > 1)
            b = 1;

        return *this;
    }

    RGB01& check()
    {
        if (r < 0)
            *this = RGB01(1, 1, 1);
        if (r > 1)
            *this = RGB01(1, 1, 1);
        if (g < 0)
            *this = RGB01(1, 1, 1);
        if (g > 1)
            *this = RGB01(1, 1, 1);
        if (b < 0)
            *this = RGB01(1, 1, 1);
        if (b > 1)
            *this = RGB01(1, 1, 1);

        return *this;
    }

    void verify() const
    {
        if (!(r >= 0 && r <= 1.) || !(g >= 0 && g <= 1.) || !(b >= 0 && b <= 1.)) {
#if defined(WIN32)
            __debugbreak();
#endif
        }
    }
};

struct RGB {
    // r, g, b - 0..255
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct YUV {
    float y;
    float u;
    float v;
};

inline YUV Rgb2Yuv(RGB01 const& rgb)
{
    // Y = .299 R + .587 G + .114 B
    // U = .492 (B - Y)
    // V = .877 (R - Y)
    YUV yuv;
    yuv.y = .299f * rgb.r + .587f * rgb.g + .114f * rgb.b;
    yuv.u = .492f * (rgb.b - yuv.y);
    yuv.v = .877f * (rgb.r - yuv.y);
    return yuv;
};

inline RGB01 Yuv2Rgb(YUV const& yuv)
{
    // Y = .299 R + .587 G + .114 B
    // U = .492 (B - Y) -> U / .492 = B - Y -> (u / .492) + Y = B
    // V = .877 (R - Y) -> V / .877 = R - Y -> (V / .877) + Y = R
    RGB01 rgb;

    rgb.b = yuv.u / .492f + yuv.y;
    rgb.r = yuv.v / .877f + yuv.y;
    rgb.g = (.299f * rgb.r + .114f * rgb.b - yuv.y) / -.587f;

    return rgb;
};

inline YCbCr rgb2YCbCr(RGB01 const& rgb)
{
    // https://en.wikipedia.org/wiki/YCbCr#ITU-R_BT.601_conversion

    auto Y = 16.f + 65.481f * rgb.r + 128.553f * rgb.g + 24.996f * rgb.b;
    auto Cb = 128.f - 37.797f * rgb.r - 74.203f * rgb.g + 112.0f * rgb.b;
    auto Cr = 128.f + 112.0f * rgb.r - 93.786f * rgb.g - 18.214f * rgb.b;

    YCbCr yuv;
    yuv.Y = uint8_t(Y);
    yuv.Cb = uint8_t(Cb);
    yuv.Cr = uint8_t(Cr);
    return yuv;
}

inline RGB YCbCr2RGB(YCbCr const& ybr)
{
    auto R = 1. * ybr.Y + 0. + 1.4 * (ybr.Cr - 128.);
    auto G = 1. * ybr.Y - .343 * (ybr.Cb - 128.) - .711 * (ybr.Cr - 128.);
    auto B = 1. * ybr.Y + 1.765 * (ybr.Cb - 128.) + 0.;

    RGB rgb = { uint8_t(R), uint8_t(G), uint8_t(B) };
    return rgb;
}

struct YDbDr {
    float y;
    float db;
    float dr;
};

inline YDbDr Yuv2YDbDr(YUV const& yuv)
{
    // https://en.wikipedia.org/wiki/YDbDr
    YDbDr retVal;

    retVal.y = yuv.y;
    retVal.db = 3.059 * yuv.u;
    retVal.dr = -2.169 * yuv.v;

    return retVal;
};

inline YUV YDbDr2Yuv(YDbDr const& ydbdr)
{
    // https://en.wikipedia.org/wiki/YDbDr
    YUV retVal;

    retVal.y = ydbdr.y;
    retVal.u = ydbdr.db / 3.059;
    retVal.v = ydbdr.dr / -2.169;

    return retVal;
};

inline YDbDr RGB2YDbDr(const RGB01& rgb)
{
    // Dr = -1.9*(R-Y) -> R = Dr / -1.9 + Y
    // Db = +1.5*(B-Y) -> B = Db / +1.5 + Y
    // Y = 0.3*R + 0.59*G + 0.11*B -> G = -(0.3*R + 0.11*B - Y) / 0.59

    YDbDr ydbdr;

    ydbdr.y = 0.3 * rgb.r + 0.59 * rgb.g + 0.11 * rgb.b;
    ydbdr.db = +1.5 * (rgb.b - ydbdr.y);
    ydbdr.dr = -1.9 * (rgb.r - ydbdr.y);

    return ydbdr;
}

inline RGB01 YDbDr2Rgb(YDbDr const& ydbdr)
{
    // Dr = -1.9*(R-Y) -> R = Dr / -1.9 + Y
    // Db = +1.5*(B-Y) -> B = Db / +1.5 + Y
    // Y = 0.3*R + 0.59*G + 0.11*B -> G = -(0.3*R + 0.11*B - Y) / 0.59

    RGB01 rgb;

    rgb.r = ydbdr.dr / -1.9 + ydbdr.y;
    rgb.b = ydbdr.db / +1.5 + ydbdr.y;
    rgb.g = -(0.3 * rgb.r + 0.11 * rgb.b - ydbdr.y) / 0.59;

    return rgb;
};

} // namespace atv
