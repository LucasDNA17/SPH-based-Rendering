#include <random>
#include <iostream>
#include <fstream>


void generateCircle(const int n_samples, const double kernelRadius)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> degree(0.0f, 2*M_PI);
    std::uniform_real_distribution<double> radii(0.0f, 5.0f);

    std::ofstream particles_file("circle.txt");

    particles_file << kernelRadius << std::endl;
    std::cout << "Generating particles..." << std::endl;
    for(int i = 0; i < n_samples; i++)
    {
        double theta = degree(gen);
        double r = radii(gen);

        particles_file << 10 + r*cos(theta) << " " << 10 + r*sin(theta) << " " << 0 << " " << 1 << " " << 1 << '\n';
    }
    particles_file.close();
    std::cout << "Done generating particles." << std::endl; 
}


void generateSphere(const int n_samples, const double kernelRadius)
{
    double x, y, z;
    double radius;
    std::cout << "Insert center coordinates: ";
    std::cin >> x >> y >> z;
    std::cout << "Insert radius: ";
    std::cin >> radius; 

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> u(0.0f, 1.0f);
    std::uniform_real_distribution<double> theta_dist(0.0f, 2.0f*M_PI);
    
    std::ofstream particles_file("sphere.txt");

    particles_file << kernelRadius << std::endl;
    std::cout << "Generating particles..." << std::endl;
    for(int i = 0; i < n_samples; i++)
    {
        double theta = theta_dist(gen);
        double phi = acos(1.0f - 2.0f*u(gen));
        double r = radius*cbrt(u(gen));

        particles_file << x + r*cos(theta)*sin(phi) << " " << y + r*sin(theta)*sin(phi) << " " << z + r*cos(phi) << " " << 1 << " " << 1 << '\n';
    }
    particles_file.close();
    std::cout << "Done generating particles." << std::endl; 
}

void generateParaboloid(const int n_samples, const double kernelRadius)
{
    double a, range_min, range_max;
    double x_center, y_center, z_center;
    std::cout << "Insert a : ";
    std::cin >> a;
    std::cout << "Insert center coordinates: ";
    std::cin >> x_center >> y_center >> z_center;
    std::cout << "Insert range for x, y: ";
    std::cin >> range_min >> range_max;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> u(0.0f, 1.0f);
    
    std::ofstream particles_file("paraboloid.txt");


    particles_file << kernelRadius << std::endl;
    std::cout << "Generating particles..." << std::endl;
    for(int i = 0; i < n_samples; i++)
    {
        double x = -1, y = -1, z = -1;
        while(x < range_min || x > range_max ||
              y < range_min || y > range_max)
        {
            double theta = 2.0f*M_PI*u(gen);
            double term = pow(1.0f + 4.0f*a*a*range_max*range_max, 1.5f);

            double r = (1.0f / (2.0f*a))*sqrt(pow(1.0f + u(gen)*(term - 1.0f), 2.0f/3.0f) - 1.0f);

            x = x_center + r*cos(theta);
            y = y_center + r*sin(theta);
            z = z_center + a*r*r;
        }

        particles_file << x << " " << y << " " << z << " " << 1000 << " " << 1 << "\n";
    }
    particles_file.close();
    std::cout << "Done generating particles." << std::endl; 
}


void generateDoubleCup(const int n_samples, const double kernelRadius)
{
    size_t range_min, range_max;
    std::cout << "Insert first min and max range: ";
    std::cin >> range_min >> range_max;

    std::ofstream particles_file("grid.txt");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> u1(range_min, range_max);

    particles_file << kernelRadius << std::endl;
    std::cout << "Generating particles..." << std::endl;

    for(int i = 0; i < n_samples; i++)
    {
        double x = u1(gen), y = u1(gen), z = u1(gen);
        particles_file << x << " " << y << " " << z << " " << 10 << " " << 1 << '\n';
    }

    std::cout << "Done generating particles.\n";

    std::cout << "Insert second min and max range: ";
    std::cin >> range_min >> range_max;

    std::uniform_real_distribution<double> u2(range_min, range_max);

    std::cout << "Generating particles...\n";
    for(int i = 0; i < n_samples; i++)
    {
        double x = u2(gen), y = u2(gen), z = u2(gen);
        particles_file << x << " " << y << " " << z << " " << 100 << " " << 1 << '\n';
    }

    particles_file.close();
    std::cout << "Done generating particles." << std::endl; 
}


