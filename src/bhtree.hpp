/*

Barnes-Hut tree class

*/

#ifndef _BHTREE_H_
#define _BHTREE_H_

#include "quad.hpp"
#include "body.hpp"

class BHTree {
    private:
        Body body; // body or aggregate body stored in this node
        Quad quadrant; // square region that the tree represents

        BHTree * NW; // tree representing northwest quadrant
        BHTree * NE;
        BHTree * SW;
        BHTree * SE;

        bool isEmpty;
        bool isInternal;

        void whichQuadrant(const Body & b); //helper function for determining the correct quadrant

    public:
        BHTree(const Quad & q); //creates a Barnes-Hut tree with no bodies, representing the given quadrant
        ~BHTree();      
        void insert(const Body & b); // inserts a body to the tree
        void updateForce(Body & b); //approximates and updates the net force acting on body b from all the bodies in a particlar tree


};

#endif