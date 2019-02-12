//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

//Header file for sprite class, declaring all methods

#ifndef SPRITE_H
#define SPRITE_H

#include <vector>
#include <QColor>
#include <QImage>

class Sprite
{
    //size vars
    int Mwidth;
    int Mheight;

    //color of each pixel stored in vector
    std::vector<QColor> Mpixels;

public:
    //Constructor taking in (width, height)
    Sprite(int, int);

    //Consider another constructor that takes in a bmp file

    //Change the color of one pixel
    void changePixelColor(int, int, QColor);

    //get the color of a pixel
    QColor getPixelColor(int, int);

    //returns width
    int getWidth();

    //returns height
    int getHeight();

    //fills sprite given color.  Best used for filling background of a sprite
    void fill(QColor);

    void resetPixels();

    void pasteQMapAt(int,int,QImage);

    std::vector<QColor> getPixels();
};

#endif // SPRITE_H
