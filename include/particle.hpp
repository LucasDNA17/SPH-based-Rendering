#pragma once

#include <vector>
#include <fstream>
#include <iostream>

typedef struct particle
{
    double x_, y_, z_;
    double m_, p_; 

    particle(double x, double y, double z, double m, double p) : x_(x), y_(y), z_(z), m_(m), p_(p) {};
    void print() const
    {
        std::cout << x_ << " " << y_ << " " << z_ << " " << m_ << " " << p_ << std::endl;
    }
} Particle;

std::vector<Particle> readParticles(const char* filepath, double *particleRadius);
double calculateParticleSpacing(const std::vector<Particle>& particles);