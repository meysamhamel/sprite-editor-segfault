//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

//Header file for viewport, declaring all methods.

#ifndef viewport_H
#define viewport_H

//this class will keep track of what portion of our sprite will be visible on the canvas
class viewport
{
private:
    int Mx, My, Mwidth, Mheight;    //top left location and width and height of the vp on a sprite
public:
    //Constructor taking in (left location, top location, width, height) args
    viewport(int, int, int, int);

    //returns left location of viewport
    int getLeft();

    //returns right location of viewport
    int getRight();

    //returns top location of viewport
    int getTop();

    //returns bot location of viewport
    int getBot();

    //returns height
    int getHeight();

    //returns width
    int getWidth();

    //sets height to given value
    void setHeight(int);

    //sets width to given value
    void setWidth(int);

    //moves entire viewport horizontally by the number of pixels entered in the argument
    //negative moves left, positive moves right
    void moveHoriz(int);

    //moves entire viewport vertically by the number of pixels entered in the argumentj
    //negative moves down, positive moves up
    void moveVert(int);

    //sets x location
    void moveXTo(int);

    //sets y location
    void moveYTo(int);
};

#endif // viewport_H
