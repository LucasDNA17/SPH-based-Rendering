#pragma once

#include "particle.hpp"
#include "kernel.hpp"
#include <memory>

class SPH
{
    private:
        float h_ = -1;
        float x_min_, x_max_, y_min_, y_max_, z_min_, z_max_;
        std::vector<Particle> particles_;
        std::shared_ptr<kernel> smoothing_kernel_ = nullptr;
        
        size_t x_grid_, y_grid_, z_grid_; 
        size_t bucket_x_, bucket_y_, bucket_z_, num_buckets_;
        std::vector<std::vector<size_t>> buckets_;
        std::vector<float> grid_values_;
        
        inline void getBucketIndexes(const float x, const float y, const float z, size_t& x_bucket, size_t& y_bucket, size_t& z_bucket) const;
        inline size_t bucketHash(const int bucket_x, const int bucket_y, const int bucket_z) const;
        inline size_t gridHash(const size_t x, const size_t y, const size_t z) const;

        void findMinMaxDimensions();

    public:
        SPH() {};
        void setKernelRadius(const float h);
        void setParticles(const std::vector<Particle>& particles);
        void setSmoothingKernel(const std::shared_ptr<kernel> W); 
        void setGrid();
        void run();
        void writeTxt(const char* filepath) const;
};