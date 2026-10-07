#include "../include/SPH.hpp"
#include "../include/kernel.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>

/*Verification functions*/
inline void checkKernelRadius(const float h)
{
    if(h <= 0)
    {
        std::cout << "Error. Kernel radius must be positive!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

inline void checkParticles(const std::vector<Particle>& particles)
{
    if(particles.size() <= 0)
    {
        std::cout << "Error. Particles vector must be nonempty!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

inline void checkSmoothingKernel(const std::shared_ptr<kernel> W)
{
    if(W == nullptr)
    {
        std::cout << "Error. Smoothing Kernel must not be null!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

inline void checkDimensions(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max)
{
    if(x_min > x_max || y_min > y_max || z_min > z_max)
    {
        std::cout << "Error. Min and max dimensions have not been correctly set!" << std::endl;
        exit(EXIT_FAILURE);
    }
}
/*End of verification functions*/



inline float squaredDistanceFromParticle(const float x, const float y, const float z, const Particle& particle)
{
    return (x - particle.x_)*(x - particle.x_) + (y - particle.y_)*(y - particle.y_) + (z - particle.z_)*(z - particle.z_);
}

/*Set functions*/
void SPH::setKernelRadius(const float h)
{
    checkKernelRadius(h);
    h_ = h;
}

void SPH::setParticles(const std::vector<Particle>& particles)
{
    checkParticles(particles);
    particles_ = particles;
}

void SPH::setSmoothingKernel(const std::shared_ptr<kernel> W)
{
    checkSmoothingKernel(W);
    smoothing_kernel_ = W;
}
/*End of set functions*/

void SPH::findMinMaxDimensions()
{
    x_min_ = particles_[0].x_; x_max_ = particles_[0].x_;
    y_min_ = particles_[0].y_; y_max_ = particles_[0].y_;
    z_min_ = particles_[0].z_; z_max_ = particles_[0].z_;
    for(const auto& particle : particles_)
    {
        if(particle.x_ < x_min_)
        {
            x_min_ = particle.x_;
        }        
        else if(particle.x_ > x_max_)
        {
            x_max_ = particle.x_;
        }

        if(particle.y_ < y_min_)
        {
            y_min_ = particle.y_;
        }        
        else if(particle.y_ > y_max_)
        {
            y_max_ = particle.y_;
        }

        if(particle.z_ < z_min_)
        {
            z_min_ = particle.z_;
        }        
        else if(particle.z_ > z_max_)
        {
            z_max_ = particle.z_;
        }
    }
}


inline void SPH::getBucketIndexes(const float x, const float y, const float z, int &x_bucket, int &y_bucket, int &z_bucket) const
{
    x_bucket = static_cast<int>(std::floor((x - x_min_)/h_));
    y_bucket = static_cast<int>(std::floor((y - y_min_)/h_));
    z_bucket = static_cast<int>(std::floor((z - z_min_)/h_));

    x_bucket = std::min(x_bucket, std::max(0, static_cast<int>(x_grid_) - 1));
    y_bucket = std::min(y_bucket, std::max(0, static_cast<int>(y_grid_) - 1));
    z_bucket = std::min(z_bucket, std::max(0, static_cast<int>(z_grid_) - 1));
}


inline size_t SPH::bucketHash(const int bucket_x, const int bucket_y, const int bucket_z) const
{
    return bucket_z*x_grid_*y_grid_ + bucket_y*x_grid_ + bucket_x;
}

void SPH::setGrid()
{
    checkParticles(particles_);
    checkKernelRadius(h_);
    findMinMaxDimensions();
    
    //adjustment
    x_min_ = static_cast<int>(std::max(std::floor(x_min_) - 2, 0.0f)); x_max_ = static_cast<int>(ceil(x_max_) + 2);
    y_min_ = static_cast<int>(std::max(std::floor(y_min_) - 2, 0.0f)); y_max_ = static_cast<int>(ceil(y_max_) + 2);
    z_min_ = static_cast<int>(std::max(std::floor(z_min_) - 2, 0.0f)); z_max_ = static_cast<int>(ceil(z_max_) + 2);

    x_min_ = std::max(static_cast<int>(x_min_), 0);
    y_min_ = std::max(static_cast<int>(y_min_), 0);
    z_min_ = std::max(static_cast<int>(z_min_), 0);
    checkDimensions(x_min_, x_max_, y_min_, y_max_, z_min_, z_max_);

    //set buckets
    x_grid_ = static_cast<int>(std::ceil((x_max_ - x_min_)/h_));
    y_grid_ = static_cast<int>(std::ceil((y_max_ - y_min_)/h_));
    z_grid_ = static_cast<int>(std::ceil((z_max_ - z_min_)/h_));
    num_buckets_ = x_grid_*y_grid_*z_grid_;
    
    //initialize buckets
    buckets_.clear();
    buckets_.resize(num_buckets_);

    for(size_t i = 0; i < particles_.size(); i++)
    {
        int bucket_x, bucket_y, bucket_z;
        getBucketIndexes(particles_[i].x_, particles_[i].y_, particles_[i].z_, bucket_x, bucket_y, bucket_z);
        
        size_t key = bucketHash(bucket_x, bucket_y, bucket_z);
        buckets_[key].push_back(i);
    }

}


void SPH::writeTxt(const char* filepath) const
{
    std::ofstream output_file(filepath);
    if(!output_file.is_open())
    {
        std::cout << "Error. Output file could not be opened when running SPH-based surface reconstruction!" << std::endl;
        exit(EXIT_FAILURE);
    }


    for(size_t i = 0; i < grid_values_.size(); i++)
    {
        for(size_t j = 0; j < grid_values_[i].size(); j++)
        {
            for(size_t k = 0; k < grid_values_[i][j].size(); k++)
            {
                output_file << i + x_min_ << " " << j + y_min_ << " " << k + z_min_ << " " << grid_values_[i][j][k] << '\n';
            }
        }
    }

    output_file.close();
}


void SPH::run()
{
    checkSmoothingKernel(smoothing_kernel_);
    checkDimensions(x_min_, x_max_, y_min_, y_max_, z_min_, z_max_);

    
    /*Initalize value_grid*/
    size_t n_x = x_max_ - x_min_ + 1;
    size_t n_y = y_max_ - y_min_ + 1;
    size_t n_z = z_max_ - z_min_ + 1;

    grid_values_.clear();
    grid_values_.resize(n_x);
    for(size_t i = 0; i < n_x; i++)
    {
        grid_values_[i].clear();
        grid_values_[i].resize(n_y);
        for(size_t j = 0; j < n_y; j++)
        {
            grid_values_[i][j].clear();
            grid_values_[i][j].resize(n_z, 0);
        }
    }

    //bucket-based grid points iteration
    auto start = std::chrono::high_resolution_clock::now();
    for(int x = x_min_; x <= x_max_; x++)
    {
        for(int y = y_min_; y <= y_max_; y++)
        {
            for(int z = z_min_; z <= z_max_; z++)
            {
                float smoothed_color_field = 0;
                
                int bucket_x, bucket_y, bucket_z;
                getBucketIndexes(x, y, z, bucket_x, bucket_y, bucket_z);

                for(int dx = -1; dx <= 1; dx++)
                {
                    for(int dy = -1; dy <= 1; dy++)
                    {
                        for(int dz = -1; dz <= 1; dz++)
                        {
                            int new_bucket_x = bucket_x + dx;
                            int new_bucket_y = bucket_y + dy;
                            int new_bucket_z = bucket_z + dz;

                            if(new_bucket_x >= 0 && new_bucket_x < static_cast<int>(x_grid_) &&
                               new_bucket_y >= 0 && new_bucket_y < static_cast<int>(y_grid_) &&
                               new_bucket_z >= 0 && new_bucket_z < static_cast<int>(z_grid_))
                            {
                                size_t bucket = bucketHash(new_bucket_x, new_bucket_y, new_bucket_z);
                                for(const size_t idx : buckets_[bucket])
                                    smoothed_color_field += smoothing_kernel_->smoothing_kernel(squaredDistanceFromParticle(x, y, z, particles_[idx]))*particles_[idx].m_/particles_[idx].p_;

                            }
                        }
                    }
                }

                float x_idx = x - x_min_, y_idx = y - y_min_, z_idx = z - z_min_; 
                grid_values_[x_idx][y_idx][z_idx] = smoothed_color_field;
            }
        }
      }

      auto end = std::chrono::high_resolution_clock::now();


    //Search first by the particles

    // auto start = std::chrono::high_resolution_clock::now();

    // for(const Particle& particle : particles_)
    // {
    //     int bucket_x, bucket_y, bucket_z;
    //     getBucketIndexes(particle.x_, particle.y_, particle.z_, bucket_x, bucket_y, bucket_z);

    //     for(int dx = -1; dx <= 1; dx++)
    //     {
    //         for(int dy = -1; dy <= 1; dy++)
    //         {
    //             for(int dz = -1; dz <= 1; dz++)
    //             {
    //                 int new_bucket_x = bucket_x + dx;
    //                 int new_bucket_y = bucket_y + dy;
    //                 int new_bucket_z = bucket_z + dz;

    //                 /*Possibly affected grid points*/

    //                 float x_min = x_min_ + h_*new_bucket_x;
    //                 int x_int_min = static_cast<int>(std::ceil(x_min));
    //                 int x_int_max = static_cast<int>(std::floor(x_min + h_));

    //                 float y_min = y_min_ + h_*new_bucket_y;
    //                 int y_int_min = static_cast<int>(std::ceil(y_min));
    //                 int y_int_max = static_cast<int>(std::floor(y_min + h_));

    //                 float z_min = z_min_ + h_*new_bucket_z;
    //                 int z_int_min = static_cast<int>(std::ceil(z_min));
    //                 int z_int_max = static_cast<int>(std::floor(z_min + h_));

    //                 for(int x = x_int_min; x <= x_int_max; x++)
    //                 {
    //                     for(int y = y_int_min; y <= y_int_max; y++)
    //                     {
    //                         for(int z = z_int_min; z <= z_int_max; z++)
    //                         {
    //                             //Index adjustement
    //                             if(x_int_min >= x_min_ && x_int_max <= x_max_ &&
    //                             y_int_min >= y_min_ && y_int_max <= y_max_ &&
    //                             z_int_min >= z_min_ && z_int_max <= z_max_)
    //                             {
    //                                 x -= x_min_; y -= y_min_; z -= z_min_;
    //                                 grid_values_[x][y][z] += smoothing_kernel_->smoothing_kernel(squaredDistanceFromParticle(x, y, z, particle))*particle.m_/particle.p_;
    //                             }
    //                         }
    //                     }
    //                 }
                
    //             }
    //         }
    //     }
    // }
    // auto end = std::chrono::high_resolution_clock::now();
    // auto execution_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    auto execution_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Execution time = "  << execution_time.count() << std::endl;
}