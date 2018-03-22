//
//  PerlinNoise.hpp
//  assignment0
//
//  Created by Xie kesong on 3/13/18.
//  Copyright © 2018 ___CSE167___. All rights reserved.
//

#ifndef PerlinNoise_hpp
#define PerlinNoise_hpp

#include <stdio.h>

class PerlinNoise
{
public:
    PerlinNoise(double persistence, double frequency, double amplitude, int octaves, int randomseed);

    // Get Height from the x y coordinates
    double GetHeight(double x, double y);
    double smoothStep(double x, double y);

private:
    
    double Interpolate(double x, double y, double a);
    double getNoise(int x, int y);
    
    double persistence, frequency, amplitude;
    int octaves, seed;
};


#endif /* PerlinNoise_hpp */
