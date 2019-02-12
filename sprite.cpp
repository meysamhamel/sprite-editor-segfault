//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

#include "sprite.h"
#include <QImage>
#include <QDebug>

//constructor
Sprite::Sprite(int width, int height)
{
    //sets size
    Mwidth = width;
    Mheight = height;

    //initialize all pixels
    for (int i = 0; i < Mwidth*Mheight; i++) {
        Mpixels.push_back(Qt::white);
    }
}

//changes the color of a pixel at x, y to color
void Sprite::changePixelColor(int x, int y, QColor color) {
    Mpixels[y*Mwidth+x] = color;
}

//returns color of a pixel at x y
QColor Sprite::getPixelColor(int x, int y) {
    int index = y*Mwidth + x;
    return Mpixels[index];
}

//returns width
int Sprite::getWidth() {
    return Mwidth;
}

//returns height
int Sprite::getHeight() {
    return Mheight;
}

void Sprite::resetPixels()
{
    //initialize all pixels
    for (int i = 0; i < Mpixels.size(); i++)
    {
        Mpixels[i] = Qt::white;
    }
}

//returns vector of pixels
std::vector<QColor> Sprite::getPixels()
{
    return Mpixels;
}

//takes an image and pastes it into the map
void Sprite::pasteQMapAt(int x,int y,QImage map) {

    for (int i = x; i < std::min(x + map.width(), Mwidth); i++) {
        for (int j = y; j < std::min(y + map.height(), Mheight); j++) {
            this->changePixelColor(i,j, map.pixel(i-x,j-y));
            //std::cout<<map.pixel(i-x, j-y) << std::endl;
        }
    }
}

//fills entire sprite one color
void Sprite::fill(QColor c) {
    for(int i = 0; i < Mpixels.size() ; i++) {
        Mpixels[i] = c;
    }
}
