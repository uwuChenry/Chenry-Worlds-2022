#include "rrlib/util/structs.hpp"
#include "rrlib/util/Vector2.hpp"
#include "rrlib/util/point2d.hpp"


using namespace RRLib;
class CubiCBezierCurve {
    Vector2 iPoint1;
    Vector2 iPoint2;

    Point2D p1;
    Point2D p2;
    Point2D c1;
    Point2D c2;


Point2D getPoint (double t) const{
    return p1*(1-t)*(1-t)*(1-t) + c1*3*(1-t)*(1-t)*t + c2*3*(1-t)*t*t + p2*t*t*t;
}

Point2D getVelocity (double t) const{
    return p1*(-3*t*t+6*t-3) + c1*(9*t*t-12*t+3) + c2* (-9*t*t+6*t) + p2 * (3*t*t);
}

Point2D getAcceleration (double t) const {
    return p1*(-6*t+6) + c1*(18*t-12) + c2*(-18*t+6) + p2*(6*t);
}

double getLengthByLength (double distPerSeg){
    double totalDist = 0;
    for (size_t i = 0; i < 200; i++)
    {
        totalDist += getPoint(i)
    }
    
}

};