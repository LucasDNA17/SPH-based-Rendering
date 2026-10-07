#pragma once

#include <vector>
#include <fstream>
#include <iostream>

typedef struct particle
{
    float x_, y_, z_;
    float m_, p_; 

    particle(float x, float y, float z, float m, float p) : x_(x), y_(y), z_(z), m_(m), p_(p) {};
    void print() const
    {
        std::cout << x_ << " " << y_ << " " << z_ << " " << m_ << " " << p_ << std::endl;
    }
} Particle;

std::vector<Particle> readParticles(const char* filepath, float *particleRadius);