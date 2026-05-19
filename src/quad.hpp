/*

Quad class to represent a square subdivision of the plane

*/

#ifndef _QUAD_H_
#define _QUAD_H_

#include <raylib.h>

class Quad {
    private:
        double x_coordinate; // lower left endpoint x coordinate
        double y_coordinate; // lower left endpoint y coordinate
        double side_len;

    public:
        Quad(double x, double y, double length);

        double side_length();
        bool contains(const double x, const double y) const;

        Quad NW();
        Quad NE();
        Quad SW();
        Quad SE();

        double getX() { return x_coordinate; }
        double getY() { return y_coordinate; }
};

#endif