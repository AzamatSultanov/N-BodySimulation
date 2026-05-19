#include "body.hpp"

Body::Body() {
    position_x = 0;
    position_y = 0;
    velocity_x = 0;
    velocity_y = 0;
    mass = 1;
    net_force_x = 0;
    net_force_y = 0;
}

Body::Body(double pos_x, double pos_y, double vel_x, double vel_y, float m, char col) {
    position_x = pos_x;
    position_y = pos_y;
    mass = m;
    velocity_x = vel_x;
    velocity_y = vel_y;
    net_force_x = 0;
    net_force_y = 0; 

    color = col;
}

bool Body::in(const Quad &q) const {
    return q.contains(position_x, position_y);
}

Body Body::add(const Body& a, const Body& b) {
    float total_mass = a.mass + b.mass;
    double center_x = (a.position_x * a.mass + b.position_x * b.mass) / total_mass;
    double center_y = (a.position_y * a.mass + b.position_y * b.mass) / total_mass;

    return Body(center_x, center_y, 0.0, 0.0, total_mass, 'a');
}

void Body::updatePos(float dt) {
    //acceleration (a = F/m)
    double acc_x = net_force_x / mass;
    double acc_y = net_force_y / mass;

    //velocity update (v2 = v1 + a*t)
    velocity_x += acc_x * dt;
    velocity_y += acc_y * dt;

    //position update
    position_x += velocity_x * dt;
    position_y += velocity_y * dt;

    net_force_x = 0;
    net_force_y = 0;
}