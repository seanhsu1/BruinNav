//
//  support.hpp
//  BruinNav
//
//  Created by Shangyu Hsu on 3/15/17.
//  Copyright © 2017 Shangyu Hsu. All rights reserved.
//

#ifndef support_h
#define support_h

#include <string>
#include "provided.h"

int compare(double a, double b);

bool operator==(const GeoCoord &g1, const GeoCoord &g2);

bool operator<(const GeoCoord &g1, const GeoCoord &g2);

bool operator>(const GeoCoord &g1, const GeoCoord &g2);

std::string directionOfLine(const GeoSegment& g);


#endif /* support_h */
