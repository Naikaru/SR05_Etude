#ifndef MATHHELPER_H
#define MATHHELPER_H

#define _USE_MATH_DEFINES

#include "math.h"

class MathHelper
{
    MathHelper();

public:
    static double fromDegToRad(double degree) {return degree * M_PI/ 180;}
    static double fromRadToDeg(double radian) {return radian * 180 / M_PI;}
    static int roundToNearestInt(float value) {return floor(value + 0.5);}
    static int roundToNearestInt(double value) {return floor(value + 0.5);}
};

#endif // MATHHELPER_H
