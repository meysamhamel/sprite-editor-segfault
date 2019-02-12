//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

//Header file; declaration of all methods in here.

#ifndef PIXELCANVAS_H
#define PIXELCANVAS_H

#include <QPixmap>
#include <QPainter>

#include <QDebug>


class PixelCanvas
{
private:
    //constant values of the canvas
    const QColor BACKGROUND_COLOR = Qt::darkGray;
    const QColor PIXEL_BORDER_COLOR = Qt::black;
    const QColor SELECTION_BORDER_COLOR = Qt::green;
    const int PIXEL_BORDER_WIDTH = 1;

    //size of each pixel stretched on the map
    int MpixelViewSize;

    //pointer to painter the maps painter
    QPainter * Mpainter;

    //bool to keep track of grid toggle
    bool Mgrid;

public:
    //create new PixelCanvas with (height, width, pixel view size)
    PixelCanvas(int, int, int);

    //Destructor
    ~PixelCanvas();

    //map to be drawn
    QPixmap* map;

    //returns entire map for painting
    QPixmap* getMap();

    //draw a pixel on map at (x, y, color)
    void drawPixel(int, int, QColor);

    //resize PixelCanvas
    void resize(int, int);

    //returns pixel view size
    int getPixelViewSize();

    //sets pixel view size
    void setPixelViewSize(int);

    //moves canvas horizontally left by val * pixel view size.  Right if negative
    void moveHoriz(int);

    //moves canvas vertically up by val * pixel view size.  Down if negative
    void moveVert(int);

    //Paints a column of pixels with the given vector of colors
    void paintColumn(int, std::vector<QColor>);

    //Paints a row of pixels with the given vector of colors
    void paintRow(int, std::vector<QColor>);

    //Draws a rectangle with given top, left, right, bot with selection color
    void drawSelectRect(int, int, int, int);

    //toggles the grid in the drawing on or off
    void toggleGrid(bool);

    //clears previous painting on canvas
    void clear();
};

#endif // PixelCanvas_H
