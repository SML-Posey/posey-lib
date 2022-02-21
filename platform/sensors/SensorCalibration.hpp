#pragma once

#include <stdint.h>

class SensorCalibration
{
    public:
        SensorCalibration(const float m = 1, const float b = 0) :
            m(m), b(b) {}

        SensorCalibration(
            const int ca, const int cb,
            const float va, const float vb)
        {
            // (cn - cb)   (vn - vb)
            // --------- = ---------
            // (ca - cb)   (va - vb)
            //
            // (va - vb)
            // --------- (cn - cb) = vn - vb
            // (ca - cb)
            //
            // m(cn - cb) = vn - vb
            // m*cn - m*cb + vb = vn
            // m*cn + b = vn
            m = (va - vb)/(ca - cb);
            b = vb - m*cb;
        }

        template <typename SrcT>
        float scale(const SrcT cn) const
        {
            return m*cn + b;
        }

    public:
        float m;
        float b;
};
