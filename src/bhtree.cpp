#include "bhtree.hpp"
#include <cmath>

#define THETA 0.7
#define G 200.0

BHTree::BHTree(const Quad & q) : quadrant(q) { //Quad function doesn't have the default constructor
    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;

    isEmpty = true;
    isInternal = false;
}

BHTree::~BHTree() {
    delete NW;
    delete NE;
    delete SW;
    delete SE;
}

void BHTree::whichQuadrant(const Body& b) {
    if (b.in(quadrant.NW())) {
        if (NW == nullptr) {
            NW = new BHTree(quadrant.NW());
        }
        NW->insert(b);
    } else if (b.in(quadrant.NE())) {
        if (NE == nullptr) {
            NE = new BHTree(quadrant.NE());
        }
        NE->insert(b);
    } else if (b.in(quadrant.SW())) {
        if (SW == nullptr) {
            SW = new BHTree(quadrant.SW());
        }
        SW->insert(b);
    } else if (b.in(quadrant.SE())) {
        if (SE == nullptr) {
            SE = new BHTree(quadrant.SE());
        }
        SE->insert(b);
    }
}

void BHTree::insert(const Body & b) {
    if(isEmpty) {
        body = b;
        isEmpty = false;
        return;
    }

    if(isInternal) {
        //calculating a new center of mass for the internal node
        body = Body::add(body, b);

        //repeating the process 
        whichQuadrant(b);
    } else { //if the node is external, we need to subdivide
        Body oldbody = body; 

        //to avoid infinite division
        if (oldbody.position_x == b.position_x && oldbody.position_y == b.position_y) {
            oldbody.position_x += 0.001;
        }
        isInternal = true;
        whichQuadrant(oldbody);
        whichQuadrant(b);

        body = Body::add(oldbody, b); //updating the center of mass and total mass
    }
}

void BHTree::updateForce(Body & b) {
     if (isEmpty) {
        return;
     }

     if(!isInternal) {
        if(body.position_x != b.position_x || body.position_y != b.position_y) { //as long as we are not calculating forces of a body on itself
            //calculating a distance between 2 bodies
            double dx = body.position_x - b.position_x;
            double dy = body.position_y - b.position_y;
            double distance = sqrt(dx*dx + dy*dy);
    
            //updating net forces of b
            double F = (G * body.mass * b.mass) / (distance * distance + 25.0); // added 0.1 to avoid division by 0 
            b.net_force_x += F * (dx / distance); //splitting the force vector into x and y components
            b.net_force_y += F * (dy / distance); 
        }

     } else {
        double s = quadrant.side_length();
        double dx = body.position_x - b.position_x;
        double dy = body.position_y - b.position_y;
        double d = sqrt(dx*dx + dy*dy);

        if (s/d < THETA ) { // in such case we will treat this internal node as a single body
            double F = (G * body.mass * b.mass) / (d * d + 25.0);
            b.net_force_x += F * (dx / d);
            b.net_force_y += F * (dy / d);
        } else { // else (if the body is close or a quandrant is still too huge) we will perform the process recursively on each child
            if (NW != nullptr) NW->updateForce(b);
            if (NE != nullptr) NE->updateForce(b);
            if (SW != nullptr) SW->updateForce(b);
            if (SE != nullptr) SE->updateForce(b);
        }
     }
}






