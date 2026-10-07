#include <iostream>
#include <vector>
#include "include/particle.hpp"
#include "include/SPH.hpp"
#include "include/generator.hpp"
#include <cmath>
#include <random>
#include <fstream>

using namespace std;


int main(int argc, char **argv)
{
    std::cout << "Reading particles...\n";
    float h;
    std::vector<Particle> particles = readParticles(argv[1], &h);
    std::cout << "Done reading particles.\n";

    SPH algo;
    
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
    algo.setGrid();
    std::cout << "Done setting grid.\n";

    std::cout << "Running algorithm...\n";
    algo.run();
    std::cout << "Done. running algorithm\n";

    std::cout << "Writing results to file...\n";
    const char *output_file = "smoothed_color_field.txt";
    algo.writeTxt(output_file);
    std::cout << "Done writing results to file." << std::endl;

    return 0;
}