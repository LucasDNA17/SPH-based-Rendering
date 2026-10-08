#include "include/generator.hpp"
#include <iostream>

#include <fstream>


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
        else if(argv[3][0] == 'g')
        {
            generateDoubleCup(n_samples, kernelRadius);
        }
    }

    

    
        
    //testGrid();
}