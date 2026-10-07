#include "../include/kernel.hpp"
#include <cmath>

poly6Kernel::poly6Kernel(const float h): kernel(h), h_squared_(h_*h_), normalization_constant(315.0f/(64.0f*M_PI))
{
    float h_cubed = h_*h_*h_;
    h_ninth_power_ = h_cubed*h_cubed*h_cubed;
}

float poly6Kernel::smoothing_kernel(const float r_squared) const
{
    if(r_squared > h_squared_)
        return 0;
    else
    {
        float diff = h_squared_ - r_squared;
        return normalization_constant*diff*diff*diff/h_ninth_power_;
    }
}