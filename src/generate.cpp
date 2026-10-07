#include "../include/generator.hpp"
#include <iostream>

#include <fstream>


void testGrid()
{
    std::ofstream particles_file("sphere.txt");
    particles_file << 1 << std::endl;

    for(int i = 1; i <= 5; i++)
    {
        for(int j = 1; j <= 5; j++)
        {
            for(int k = 1; k <= 5; k++)
            {
                particles_file << i << " " << j << " " << k << " " << 1 << " " << 1 << std::endl;
            }
        }
    }

    particles_file.close();
}


int main(int argc, char** argv)
{
    if(argc == 4)
    {
        int n_samples = atoi(argv[1]);
        float kernelRadius = atof(argv[2]);
        if(argv[3][0] == 'c')
        {
            generateCircle(n_samples, kernelRadius);
        }
        else if(argv[3][0] == 's')
        {
            generateSphere(n_samples, kernelRadius);
        }
        else if(argv[3][0] == 'p')
        {
            generateParaboloid(n_samples, kernelRadius);
        }
    }
    else
    {
        float kernelRadius = atof(argv[1]);
        if(argv[2][0] == 'g')
        {
            generateDoubleCup(kernelRadius);
        }
    }
    

    
        
    //testGrid();
}