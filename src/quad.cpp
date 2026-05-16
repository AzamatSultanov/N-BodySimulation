#include "quad.hpp"

Quad::Quad(double x, double y, double length) {
    x_coordinate = x;
    y_coordinate = y;
    side_len = length;
}

double Quad::side_length() {
    return side_len;
}

bool Quad::contains(const double x, const double y) const {
    return (x >= x_coordinate && x <= x_coordinate + side_len &&
            y >= y_coordinate && y <= y_coordinate + side_len);
}

Quad Quad::NW() {
    double half_len = side_len / 2.0;
    return Quad(x_coordinate, y_coordinate + half_len, half_len);
}

Quad Quad::NE() {
    double half_len = side_len / 2.0;
    return Quad(x_coordinate + half_len, y_coordinate + half_len, half_len);
}

Quad Quad::SW() {
    double half_len = side_len / 2.0;
    return Quad(x_coordinate, y_coordinate, half_len);
}

Quad Quad::SE() {
    double half_len = side_len / 2.0;
    return Quad(x_coordinate + half_len, y_coordinate, half_len);
}