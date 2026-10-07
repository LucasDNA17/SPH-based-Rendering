#include "../include/particle.hpp"

inline bool checkParticle(float x, float y, float z, float m, float p)
{
    if(x < 0 || y < 0 || z < 0 || m < 0 || p < 0)
    {
        return false;
    }

    return true;
}

std::vector<Particle> readParticles(const char* filepath, float *kernelRadius)
{
    std::ifstream input_file(filepath);
    if(!input_file.is_open())
    {
        std::cout << "Error. Particles input file could not be opened!" << std::endl;
        exit(EXIT_FAILURE);
    }

    float x, y, z, m, p;
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