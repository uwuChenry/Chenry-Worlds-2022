#pragma once

namespace RRLib {

class Point2D{
    double x;
    double y;

    public:
    Point2D();
    Point2D(double ix, double iy);
    

    Point2D operator+(const Point2D& rhs);
    Point2D operator-(const Point2D& rhs);
    Point2D operator*(double scalar) const;
    Point2D operator/(double scalar);



};

}
