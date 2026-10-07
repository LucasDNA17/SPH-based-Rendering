#pragma once

class kernel
{
    protected:
        float h_;

    public:
        kernel(const float h) : h_(h) {};
        virtual float smoothing_kernel(const float squared_r) const = 0;
};


class poly6Kernel : public kernel
{
    private:
        float h_squared_, h_ninth_power_, normalization_constant;

    public:
        poly6Kernel(const float h);
        float smoothing_kernel(const float squared_r) const;
};