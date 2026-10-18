#pragma once

class kernel
{
    protected:
        double h_;

    public:
        kernel(const double h) : h_(h) {};
        virtual double smoothing_kernel(const double squared_r) const = 0;
};


class poly6Kernel : public kernel
{
    private:
        double h_squared_, h_cubed_, normalization_constant;

    public:
        poly6Kernel(const double h);
        double smoothing_kernel(const double squared_r) const;
};
