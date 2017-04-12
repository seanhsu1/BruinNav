//
//  support.cpp
//  BruinNav
//
//  Created by Shangyu Hsu on 3/15/17.
//  Copyright © 2017 Shangyu Hsu. All rights reserved.
//

#include "support.h"
#include "provided.h"
#include <cmath>

int compare(double a, double b)
{
    double diff = a-b;
    if(fabs(diff)<.00000001)
        return 0;
    else if (diff<0)
        return -1;
    else
        return 1;
}

bool operator==(const GeoCoord &g1, const GeoCoord &g2)
{
    return ((compare(g1.latitude,g2.latitude)==0)&&(compare(g1.longitude,g2.longitude==0)));
}

bool operator<(const GeoCoord &g1, const GeoCoord &g2)
{
    if (compare(g1.latitude,g2.latitude)==1)
        return false;
    else if((compare(g1.latitude,g2.latitude)==0) && (compare(g1.longitude,g2.longitude>-1)))
        return false;
    return true;
}

bool operator>(const GeoCoord &g1, const GeoCoord &g2)
{
    if (compare(g1.latitude,g2.latitude)==-1)
        return false;
    else if((compare(g1.latitude,g2.latitude)==0) && (compare(g1.longitude,g2.longitude<1)))
        return false;
    return true;
}

std::string directionOfLine(const GeoSegment& g)
{
    double angle = angleOfLine(g);
    if(angle<=22.5)
        return "east";
    else if (angle<=67.5)
        return "northeast";
    else if (angle<=112.5)
        return "north";
    else if (angle<=157.5)
        return "northwest";
    else if (angle<=202.5)
        return "west";
    else if (angle<=247.5)
        return "southwest";
    else if (angle<=292.5)
        return "south";
    else if (angle<=337.5)
        return "southeast";
    else
        return "east";
}