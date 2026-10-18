#include "../include/particle.hpp"

#include <cmath>

inline bool checkParticle(double x, double y, double z, double m, double p)
{
    if(x < 0 || y < 0 || z < 0 || m < 0 || p < 0)
    {
        return false;
    }

    return true;
}

std::vector<Particle> readParticles(const char* filepath, double *kernelRadius)
{
    std::ifstream input_file(filepath);
    if(!input_file.is_open())
    {
        std::cout << "Error. Particles input file could not be opened!" << std::endl;
        exit(EXIT_FAILURE);
    }

    double x, y, z, m, p;
    std::vector<Particle> particles;

    input_file >> *(kernelRadius);
    while(input_file >> x >> y >> z >> m >> p)
    {
        if(!checkParticle(x, y, z, m, p))
        {
            std::cout << "Error. Particle properties should not be negative!" << std::endl;
            input_file.close();
            exit(EXIT_FAILURE);
        }
        
        Particle particle(x, y, z, m, p);
        particles.push_back(particle);
    }

    input_file.close();
    return particles;
}


inline double calculateParticleDist(const Particle& particle1, const Particle& particle2)
{
    double dx = particle1.x_ - particle2.x_;
    double dy = particle1.y_ - particle2.y_;
    double dz = particle1.z_ - particle2.z_;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

double calculateParticleSpacing(const std::vector<Particle>& particles)
{
    double average_spacing = 0;

    size_t n = particles.size();
    for(size_t i = 0; i < n; i++)
    {
        for(size_t j = i + 1; j < n; j++)
        {
            average_spacing += calculateParticleDist(particles[i], particles[j]);
        }
    }

    average_spacing /= (n*(n - 1)/2);
    return average_spacing;
}