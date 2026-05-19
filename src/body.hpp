/*

Body class

*/

#ifndef _BODY_H_
#define _BODY_H_

#include "quad.hpp"

class Body {
    public:
        float mass;
        double position_x, position_y;
        double velocity_x, velocity_y;
        double net_force_x, net_force_y;

        char color;

        Body(); //position, velocity, and net force = (0,0), mass = 1
        Body(double pos_x, double pos_y, double vel_x, double vel_y, float m, char col);

        bool in(const Quad &q) const; //checks if a body is inside a quadrant
        static Body add(const Body& a, const Body& b); //returns new body that represents the center-of-mass of the two bodies a and b
        void updatePos(float dt);
};


#endif