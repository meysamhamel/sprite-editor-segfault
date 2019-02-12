//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

#include "viewport.h"

//constructor
viewport::viewport(int x,int y, int width, int height)
{
    Mx = x;
    My = y;
    Mheight = height;
    Mwidth = width;
}

//returns viewport left x value
int viewport::getLeft() {
    return Mx;
}

//returns viewport right x value
int viewport::getRight() {
    return Mx + Mwidth;
}

//returns viewport top y value
int viewport::getTop() {
    return My;
}

//returns viewport bot y value
int viewport::getBot() {
    return My + Mheight;
}

//returns viewport height
int viewport::getHeight() {
    return Mheight;
}

//returns viewport width
int viewport::getWidth() {
    return Mwidth;
}

//sets viewport height
void viewport::setHeight(int height) {
    Mheight = height;
}

//sets viewport width
void viewport::setWidth(int width) {
    Mwidth = width;
}

//moves viewport horizontally by dist
void viewport::moveHoriz(int dist){
    Mx += dist;
}

//moves viewport vertically by dist
void viewport::moveVert(int dist) {
    My += dist;
}

//moves viewport x location to new location
void viewport::moveXTo(int x) {
    Mx = x;
}

//moves viewport y location to new location
void viewport::moveYTo(int y) {
    My = y;
}
