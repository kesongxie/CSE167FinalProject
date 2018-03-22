//
//  PerlinNoise.cpp
//  assignment0
//
//  Created by Xie kesong on 3/13/18.
//  Copyright © 2018 ___CSE167___. All rights reserved.
//

#include "PerlinNoise.hpp"
#include <math.h>

PerlinNoise::PerlinNoise(double persistence, double frequency, double amplitude, int octaves, int randomseed)
{
    this->octaves = octaves;
    this->persistence = persistence;
    this->frequency = frequency;
    this->seed = randomseed;
    this->amplitude  = amplitude;
}

double PerlinNoise::GetHeight(double x, double y)
{
    double fre = frequency;
    double t = 0.0f;
    double am = 1;
    
    for(int k = 0; k < octaves; k++)
    {
        t = t + smoothStep(y * fre + seed, x * fre + seed) * am;
        fre = fre * 2;
        am *= persistence;
    }
    return amplitude * t;
}

double PerlinNoise::smoothStep(double x, double y)
{
    int Xint = (int)x;
    int Yint = (int)y;
    double Xfrac = x - Xint;
    double Yfrac = y - Yint;
    
    //noise values
    double n_1 = getNoise(Xint-1, Yint-1);
    double n_2 = getNoise(Xint+1, Yint-1);
    double n_3 = getNoise(Xint-1, Yint+1);
    double n_4 = getNoise(Xint+1, Yint+1);
    double n_5 = getNoise(Xint-1, Yint);
    double n_6 = getNoise(Xint+1, Yint);
    double n_7 = getNoise(Xint, Yint-1);
    double n_9 = getNoise(Xint, Yint+1);
    double n_10 = getNoise(Xint, Yint);
    double n_11 = getNoise(Xint+2, Yint-1);
    double n_12 = getNoise(Xint+2, Yint+1);
    double n_16 = getNoise(Xint+2, Yint);
    double n_17 = getNoise(Xint-1, Yint+2);
    double n_18 = getNoise(Xint+1, Yint+2);
    double n_19 = getNoise(Xint, Yint+2);
    
    double n34 = getNoise(Xint+2, Yint+2);
    
    //get noise from the nearest 4 points and interpolate
    double x0y0 = 0.0625*(n_1+n_2+n_3+n_4) + 0.125*(n_5+n_6+n_7+n_9) + 0.25*(n_10);
    double x1y0 = 0.0625*(n_7+n_11+n_9+n_12) + 0.125*(n_10+n_16+n_2+n_4) + 0.25*(n_6);
    double x0y1 = 0.0625*(n_5+n_6+n_17+n_18) + 0.125*(n_3+n_4+n_10+n_19) + 0.25*(n_9);
    double x1y1 = 0.0625*(n_10+n_16+n_19+n34) + 0.125*(n_9+n_12+n_6+n_18) + 0.25*(n_4);
    
    return Interpolate(Interpolate(x0y0, x1y0, Xfrac), Interpolate(x0y1, x1y1, Xfrac), Yfrac);
}

double PerlinNoise::Interpolate(double x, double y, double a)
{
    double negA = 1.0 - a;
    double negASqr = negA * negA;
    double fac1 = 3.0 * (negASqr) - 2.0 * (negASqr * negA);
    double aSqr = a * a;
    double fac2 = 3.0 * aSqr - 2.0 *  (aSqr * a);
    return x * fac1 + y * fac2;
}

double PerlinNoise::getNoise(int x, int y)
{
    int rad = 1032; // any random number to scale y
    int n = x + y * rad;
    int t = (n * (n * n * 34561)) & 0x6fffffff;
    return 1.0 - double(t) * 0.87492048e-9;
}
