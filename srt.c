extern const int SRT_4_LUT_P5[128][16];
extern const int SRT_4_LUT_P5_FDIV[128][16];

#define FLOAT64_S   0x8000000000000000
#define FLOAT64_E   0x7ff0000000000000
#define FLOAT64_M   0x000fffffffffffff
#define FLOAT64_V   0x7fffffffffffffff
#define FLOAT64_EXP(b)  ((b & FLOAT64_E) >> 52)
#define FLOAT64_FRAC(b) (b & FLOAT64_M)
#define FLOAT64_SX(b)   ((b & FLOAT64_M) + (!!(b & FLOAT64_E) << 52))
#define FLOAT64_BIAS    1023
#define FLOAT64_IS_QUIET    0x0008000000000000

#ifdef _MSC_VER
#include <intrin.h>
#endif

#ifdef __GNUC__
#include <x86intrin.h>
#elif __MINGW64__
#include <x86intrin.h>
#elif __MINGW32__
#include <x86intrin.h>
#endif

#include <stdio.h>

double div1(double x, double y)
{
    union {
        double f;
        long long i;
    } a, b, z;

    a.f = x, b.f = y;
    z.i = (a.i ^ b.i) & FLOAT64_S;

    if ((b.i & FLOAT64_V) == 0)
    {
        if ((a.i & FLOAT64_V) == 0)
            z.i |= FLOAT64_E | FLOAT64_IS_QUIET;
        else if ((FLOAT64_EXP(a.i) == 0x7ff) && FLOAT64_FRAC(a.i))
            z.i |= FLOAT64_E | FLOAT64_FRAC(a.i);
        else
            z.i |= FLOAT64_E;

        return z.f;
    }
    else if (FLOAT64_EXP(b.i) == 0x7ff)
    {
        if (FLOAT64_EXP(a.i) == 0x7ff)
        {
            if (FLOAT64_FRAC(a.i) || FLOAT64_FRAC(b.i))
                z.i |= FLOAT64_E | FLOAT64_FRAC(a.i) | FLOAT64_FRAC(b.i);
            else
                z.i |= FLOAT64_E | FLOAT64_IS_QUIET;
        }

        return z.f;
    }

    if (FLOAT64_EXP(a.i) == 0x7ff)
    {
        if (FLOAT64_FRAC(a.i))
            z.i |= FLOAT64_E | FLOAT64_FRAC(a.i);
        else
            z.i |= FLOAT64_E;

        return z.f;
    }

    int e_x, e_y, shamt;
    long long quot, q, r, d, c;

    e_x = FLOAT64_EXP(a.i) + !!FLOAT64_EXP(a.i);
    e_y = FLOAT64_EXP(b.i) + !!FLOAT64_EXP(b.i);
    e_x -= e_y;
    e_x += FLOAT64_BIAS;

    { // normalize r
        r = FLOAT64_FRAC(a.i) + ((long long)!!FLOAT64_EXP(a.i) << 52);
        shamt = __lzcnt64(r) - 11;
        r <<= shamt, e_x -= shamt;
    }

    { // normalize d
        d = FLOAT64_FRAC(b.i) + ((long long)!!FLOAT64_EXP(b.i) << 52);
        shamt = __lzcnt64(d) - 11;
        d <<= shamt, e_x += shamt;
    }

    quot = 0, c = 0;

    for (int i = 0; i < 28; i++)
    {
        long long r_ = r, c_ = c, _r, _q, qd;

        _r = (r >> 49) + (c >> 49);
        q = SRT_4_LUT_P5[_r & 0x7f][(d >> 48) & 0xf];
        _q = (q < 0) ? -q : q;
        qd = ((_q * d) ^ -(q > 0));

        /*
        printf("%02d %02x %x %+2d\n",
        i, _r & 0x7f, (d >> 48) & 0xf,
        SRT_4_LUT_P5[_r & 0x7f][(d >> 48) & 0xf]);
        */
        
        r = c_ ^ r_ ^ qd;
        c = (((c_ & r_) | (c_ & qd) | (r_ & qd)) << 1) | (q > 0);

        quot = quot * 4 + q;
        r *= 4, c *= 4;
    }

    { // normalize quot + GRS
        r += c;         // gather r
        
        if (quot & 0x40000000000000)
        {
            quot <<= 1;

            // tie breaking: (RS) 00 -> 01 / 11
            if (quot & 0x3); else if (r < 0) quot -= 2;
            quot = quot | (!!r);
        }
        else
        {
            q = SRT_4_LUT_P5[(r >> 49) & 0x7f][(d >> 48) & 0xf];
            r -= q * d;
            quot = quot * 4 + q;

            // tie breaking: (RS) 00 -> 01 / 11
            if (q == 0) if (r < 0) quot -= 2;
            quot = quot | (!!r), e_x -= 1;
        }
    }

    if (e_x <= 0)
    {
        if (e_x < -52)
            return z.f;

        quot = (quot >> -e_x) | !!(quot & (FLOAT64_M >> (52 + e_x)));
        // RTE rounding
        quot += (quot & 0x4) * !!(quot & 0xb);

        if (quot & 0x100000000000000)
            e_x = 1;
        else
            e_x = 0;

        quot >>= 3;
    }
    else
    {
        // RTE rounding
        quot += (quot & 0x4) * !!(quot & 0xb);
        
        if (quot & 0x100000000000000)
            quot = 0, e_x += 1;
        else
            quot >>= 3;
    }

    if (e_x >= 0x7ff)
        z.i |= FLOAT64_E;
    else
        z.i |= ((long long)e_x << 52) | (quot & FLOAT64_M);

    return z.f;
}

// #include <stdio.h>

double div2(double x, double y)
{
    union {
        double f;
        long long i;
    } a, b, z;

    a.f = x, b.f = y;
    z.i = (a.i ^ b.i) & FLOAT64_S;

    if ((b.i & FLOAT64_V) == 0)
    {
        if ((a.i & FLOAT64_V) == 0)
            z.i |= FLOAT64_E | FLOAT64_IS_QUIET;
        else if ((FLOAT64_EXP(a.i) == 0x7ff) && FLOAT64_FRAC(a.i))
            z.i |= FLOAT64_E | FLOAT64_FRAC(a.i);
        else
            z.i |= FLOAT64_E;

        return z.f;
    }
    else if (FLOAT64_EXP(b.i) == 0x7ff)
    {
        if (FLOAT64_EXP(a.i) == 0x7ff)
        {
            if (FLOAT64_FRAC(a.i) || FLOAT64_FRAC(b.i))
                z.i |= FLOAT64_E | FLOAT64_FRAC(a.i) | FLOAT64_FRAC(b.i);
            else
                z.i |= FLOAT64_E | FLOAT64_IS_QUIET;
        }

        return z.f;
    }

    if (FLOAT64_EXP(a.i) == 0x7ff)
    {
        if (FLOAT64_FRAC(a.i))
            z.i |= FLOAT64_E | FLOAT64_FRAC(a.i);
        else
            z.i |= FLOAT64_E;

        return z.f;
    }

    int e_x, e_y, shamt;
    long long quot, q, r, d, c;

    e_x = FLOAT64_EXP(a.i) + !!FLOAT64_EXP(a.i);
    e_y = FLOAT64_EXP(b.i) + !!FLOAT64_EXP(b.i);
    e_x -= e_y;
    e_x += FLOAT64_BIAS;

    { // normalize r
        r = FLOAT64_FRAC(a.i) + ((long long)!!FLOAT64_EXP(a.i) << 52);
        shamt = __lzcnt64(r) - 11;
        r <<= shamt, e_x -= shamt;
    }

    { // normalize d
        d = FLOAT64_FRAC(b.i) + ((long long)!!FLOAT64_EXP(b.i) << 52);
        shamt = __lzcnt64(d) - 11;
        d <<= shamt, e_x += shamt;
    }

    quot = 0, c = 0;

    for (int i = 0; i < 28; i++)
    {
        long long r_ = r, c_ = c, _r, _q, qd;

        _r = (r >> 49) + (c >> 49);
        q = SRT_4_LUT_P5_FDIV[_r & 0x7f][(d >> 48) & 0xf];
        _q = (q < 0) ? -q : q;
        qd = ((_q * d) ^ -(q > 0));

        /*
        printf("%02d %02x %x %+2d\n",
        i, _r & 0x7f, (d >> 48) & 0xf,
        SRT_4_LUT_P5_FDIV[_r & 0x7f][(d >> 48) & 0xf]);
        */
        
        r = c_ ^ r_ ^ qd;
        c = (((c_ & r_) | (c_ & qd) | (r_ & qd)) << 1) | (q > 0);

        quot = quot * 4 + q;
        r *= 4, c *= 4;
    }

    { // normalize quot + GRS
        r += c;         // gather r
        
        if (quot & 0x40000000000000)
        {
            quot <<= 1;

            // tie breaking: (RS) 00 -> 01 / 11
            if (quot & 0x3); else if (r < 0) quot -= 2;
            quot = quot | (!!r);
        }
        else
        {
            q = SRT_4_LUT_P5[(r >> 49) & 0x7f][(d >> 48) & 0xf];
            r -= q * d;
            quot = quot * 4 + q;

            // tie breaking: (RS) 00 -> 01 / 11
            if (q == 0) if (r < 0) quot -= 2;
            quot = quot | (!!r), e_x -= 1;
        }
    }

    if (e_x <= 0)
    {
        if (e_x < -52)
            return z.f;

        quot = (quot >> -e_x) | !!(quot & (FLOAT64_M >> (52 + e_x)));
        // RTE rounding
        quot += (quot & 0x4) * !!(quot & 0xb);

        if (quot & 0x100000000000000)
            e_x = 1;
        else
            e_x = 0;

        quot >>= 3;
    }
    else
    {
        // RTE rounding
        quot += (quot & 0x4) * !!(quot & 0xb);
        
        if (quot & 0x100000000000000)
            quot = 0, e_x += 1;
        else
            quot >>= 3;
    }

    if (e_x >= 0x7ff)
        z.i |= FLOAT64_E;
    else
        z.i |= ((long long)e_x << 52) | (quot & FLOAT64_M);

    return z.f;
}
