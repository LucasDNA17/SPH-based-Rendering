#include "../include/SPH.hpp"
#include "../include/kernel.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>

/*Verification functions*/
inline void checkKernelRadius(const double h)
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

inline void checkDimensions(const double x_min, const double x_max, const double y_min, const double y_max, const double z_min, const double z_max, const double grid_spacing)
{
    if(x_min > x_max || y_min > y_max || z_min > z_max || grid_spacing < 0)
    {
        std::cout << "Error. Min, max and spacing dimensions have not been correctly set!" << std::endl;
        exit(EXIT_FAILURE);
    }
}



inline double squaredDistanceFromParticle(const double x, const double y, const double z, const Particle& particle)
{
    return (x - particle.x_)*(x - particle.x_) + (y - particle.y_)*(y - particle.y_) + (z - particle.z_)*(z - particle.z_);
}

/*Set functions*/
void SPH::setKernelRadius(const double h)
{
    checkKernelRadius(h);
    h_ = h;
    std::cout << "h = " << h_ << std::endl;
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


inline void SPH::getBucketIndexes(const double x, const double y, const double z, size_t &x_bucket, size_t &y_bucket, size_t &z_bucket) const
{
    x_bucket = static_cast<size_t>(std::floor((x - x_min_)/h_));
    y_bucket = static_cast<size_t>(std::floor((y - y_min_)/h_));
    z_bucket = static_cast<size_t>(std::floor((z - z_min_)/h_));

    x_bucket = std::min(x_bucket, std::max(static_cast<size_t>(0), static_cast<size_t>(bucket_x_) - 1));
    y_bucket = std::min(y_bucket, std::max(static_cast<size_t>(0), static_cast<size_t>(bucket_y_) - 1));
    z_bucket = std::min(z_bucket, std::max(static_cast<size_t>(0), static_cast<size_t>(bucket_z_) - 1));
}


inline size_t SPH::bucketHash(const int bucket_x, const int bucket_y, const int bucket_z) const
{
    return bucket_z*bucket_x_*bucket_y_ + bucket_y*bucket_x_ + bucket_x;
}

void SPH::setGrid(const double x_min, const double x_max, const double y_min, const double y_max, const double z_min, const double z_max, const double grid_spacing)
{
    checkDimensions(x_min, x_max, y_min, y_max, z_min, z_max, grid_spacing);
    checkKernelRadius(h_);

    x_min_ = x_min; x_max_ = x_max;
    y_min_ = y_min; y_max_ = y_max;
    z_min_ = z_min; z_max_ = z_max;
    grid_spacing_ = grid_spacing;

    /*adjust the dimensions for a regular grid*/
    x_grid_ = static_cast<int>(std::ceil((x_max_ - x_min_)/grid_spacing_)) + 1;
    x_max_ = x_min_ + grid_spacing_*x_grid_;

    y_grid_ = static_cast<int>(std::ceil((y_max_ - y_min_)/grid_spacing_)) + 1;
    y_max_ = y_min_ + grid_spacing_*y_grid_;

    z_grid_ = static_cast<int>(std::ceil((z_max_ - z_min_)/grid_spacing_)) + 1;
    z_max_ = z_min_ + grid_spacing_*z_grid_;


    std::cout << "x_min, x_max, x_grid = " << x_min_ << "," << x_max_ << "," << x_grid_ << '\n';
    std::cout << "y_min, y_max, y_grid = " << y_min_ << "," << y_max_ << "," << y_grid_ << '\n';
    std::cout << "z_min, z_max, z_grid = " << z_min_ << "," << z_max_ << "," << z_grid_ << '\n';
    std::cout << "grid_spacing = " << grid_spacing_ << '\n';
    std::cout << "num_points = " << x_grid_*y_grid_*z_grid_ << '\n'; 

    /*set the grid*/
    grid_values_.clear();
    grid_values_.resize(x_grid_*y_grid_*z_grid_);

    /*set buckets*/
    bucket_x_ = static_cast<size_t>(std::ceil((x_max_ - x_min_)/h_));
    bucket_y_ = static_cast<size_t>(std::ceil((y_max_ - y_min_)/h_));
    bucket_z_ = static_cast<size_t>(std::ceil((z_max_ - z_min_)/h_));
    num_buckets_ = bucket_x_*bucket_y_*bucket_z_;

    std::cout << "bucket_x = " << bucket_x_ << '\n';
    std::cout << "bucket_y = " << bucket_y_ << '\n';
    std::cout << "bucket_z = " << bucket_z_ << '\n';
    std::cout << "num_buckets = " << num_buckets_ << '\n';


    //initialize buckets
    buckets_.clear();
    buckets_.resize(num_buckets_);

    for(size_t i = 0; i < particles_.size(); i++)
    {
        size_t bucket_x, bucket_y, bucket_z;
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

    /*Write the grid configurations*/
    output_file << x_min_ << " " << x_max_ << '\n';
    output_file << y_min_ << " " << y_max_ << '\n';
    output_file << z_min_ << " " << z_max_ << '\n';
    output_file << grid_spacing_ << '\n';

    for(size_t idx_x = 0; idx_x < x_grid_; idx_x++)
    for(size_t idx_y = 0; idx_y < y_grid_; idx_y++)
    for(size_t idx_z = 0; idx_z < z_grid_; idx_z++)
    {
        double x = x_min_ + grid_spacing_*idx_x;
        double y = y_min_ + grid_spacing_*idx_y;
        double z = z_min_ + grid_spacing_*idx_z;   
        size_t idx = gridHash((x - x_min_)/grid_spacing_, (y - y_min_)/grid_spacing_, (z - z_min_)/grid_spacing_);
        output_file << x << " " << y << " " << z << " " << grid_values_[idx] << '\n';
    }

    output_file.close();
}


inline size_t SPH::gridHash(const size_t x, const size_t y, const size_t z) const
{
    return z*x_grid_*y_grid_ + y*x_grid_ + x;
}

void SPH::run()
{
    checkSmoothingKernel(smoothing_kernel_);
    checkDimensions(x_min_, x_max_, y_min_, y_max_, z_min_, z_max_, grid_spacing_);

    //bucket-based grid points iteration
    auto start = std::chrono::high_resolution_clock::now();
    double h_squared = h_*h_;
    for(size_t idx_x = 0; idx_x < x_grid_; idx_x++)
    {
        for(size_t idx_y = 0; idx_y < y_grid_; idx_y++)
        {
            for(size_t idx_z = 0; idx_z < z_grid_; idx_z++)
            {
                double x = x_min_ + grid_spacing_*idx_x;
                double y = y_min_ + grid_spacing_*idx_y;
                double z = z_min_ + grid_spacing_*idx_z;
                double smoothed_color_field = 0;
                
                size_t bucket_x, bucket_y, bucket_z;
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

                            if(new_bucket_x >= 0 && new_bucket_x < static_cast<int>(bucket_x_) &&
                               new_bucket_y >= 0 && new_bucket_y < static_cast<int>(bucket_y_) &&
                               new_bucket_z >= 0 && new_bucket_z < static_cast<int>(bucket_z_))
                            {
                                size_t bucket = bucketHash(new_bucket_x, new_bucket_y, new_bucket_z);            
                                for(const size_t idx : buckets_[bucket])
                                {
                                    double distance_from_particle = squaredDistanceFromParticle(x, y, z, particles_[idx]);
                                    if(distance_from_particle <= h_squared)
                                    {
                                        smoothed_color_field += smoothing_kernel_->smoothing_kernel(distance_from_particle)*particles_[idx].m_/particles_[idx].p_;
                                    }
                                }
                            }
                        }
                    }
                }

                size_t x_idx = (size_t)((x - x_min_)/grid_spacing_), y_idx = (size_t)((y - y_min_)/grid_spacing_), z_idx = (size_t)((z - z_min_)/grid_spacing_); 
                size_t idx = gridHash(x_idx, y_idx, z_idx);
                grid_values_[idx] = smoothed_color_field;
            }
        }
      }

    auto end = std::chrono::high_resolution_clock::now();
    auto execution_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Execution time = "  << execution_time.count()*1e-9 << " s " << std::endl;
}