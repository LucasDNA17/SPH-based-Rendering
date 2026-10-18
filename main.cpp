#include <iostream>
#include <vector>
#include <fstream>
#include "include/particle.hpp"
#include "include/SPH.hpp"

using namespace std;

int main(int argc, char **argv)
{
    std::cout << "Reading particles...\n";
    double particle_spacing;
    std::vector<Particle> particles = readParticles(argv[1], &particle_spacing);
    std::cout << "Done reading particles.\n";

    SPH algo;

    double particle_spacing_multiplier = atof(argv[3]);
    double h = particle_spacing_multiplier*particle_spacing;
    std::cout << "Setting kernel radius...\n";
    algo.setKernelRadius(h);
    std::cout << "Done setting kernel radius...\n";
    
    std::cout << "Setting particles...\n";
    algo.setParticles(particles);
    std::cout << "Done setting particles.\n";

    std::cout << "Setting kernel function...\n";
    std::shared_ptr<kernel> smoothing_kernel(new poly6Kernel(h));
    algo.setSmoothingKernel(smoothing_kernel);
    std::cout << "Done setting kernel function.\n";
    
    std::cout << "Setting grid...\n";
    algo.setGrid(0, 4, 0, 2, 0, 3, 0.02f);
    std::cout << "Done setting grid.\n";

    std::cout << "Running algorithm...\n";
    algo.run();
    std::cout << "Done. running algorithm\n";

    std::cout << "Writing results to file...\n";
    algo.writeTxt(argv[2]);
    std::cout << "Done writing results to file." << std::endl;

    return 0;
}