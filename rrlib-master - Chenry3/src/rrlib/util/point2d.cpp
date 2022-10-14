#pragma once

#include "okapi/api/chassis/model/chassisModel.hpp"
#include "okapi/api/units/QAngle.hpp"
#include "okapi/api/units/QLength.hpp"
#include "rrlib/util/point2d.hpp"

namespace RRLib {
using namespace okapi::literals;



Point2D::Point2D(){
    x = 0; y=0;
}

Point2D::Point2D(double ix, double iy){
    x = ix;
    y = iy;
    
}

Point2D Point2D::operator*(double scalar) const{
    return {x * scalar, y * scalar};
}

}