/*

Quad class to represent a square subdivision of the plane

*/

#ifndef _QUAD_H_
#define _QUAD_H_

#include <raylib.h>

class Quad {
    private:
        double x_coordinate; //lower left endpoint x coordinate
        double y_coordinate; //lower left endpoint y coordinate
        double side_len;

    public:
        Quad(double x, double y, double length);

        double side_length(); //returns the length of a side of the square.
        bool contains(const double x, const double y) const; //returns true if (x, y) is in the region, and false otherwise.

        Quad NW(); //create and return a new Quad representing a sub-quadrant of the invoking quadrant.
        Quad NE();
        Quad SW();
        Quad SE();

        double getX(){
            return x_coordinate;
        };
        double getY(){
            return y_coordinate;
        };
};


#endif