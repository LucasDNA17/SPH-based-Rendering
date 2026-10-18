#include "../include/kernel.hpp"
#include <cmath>

poly6Kernel::poly6Kernel(const double h): kernel(h), h_squared_(h_*h_), h_cubed_(h_*h_squared_), normalization_constant(315.0f/(64.0f*M_PI))
{}


double poly6Kernel::smoothing_kernel(const double r_squared) const
{
    if(r_squared > h_squared_)
        return 0;
    else
    {
        double diff = 1 - (r_squared/h_squared_);
        return normalization_constant*diff*diff*diff/h_cubed_;
    }
}