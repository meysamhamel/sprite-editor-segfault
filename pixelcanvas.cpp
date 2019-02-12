//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

#include "pixelcanvas.h"

//constructor
PixelCanvas::PixelCanvas(int width, int height, int pixelViewSize)
{
    //sets pixel size
    MpixelViewSize = pixelViewSize;

    //creates new canvas
    map = new QPixmap(width, height);

    //fills background of canvas
    map->fill(BACKGROUND_COLOR);

    //creates painter object
    Mpainter = new QPainter(map);

    //turns grid on
    Mgrid = true;
}

//destructor
PixelCanvas::~PixelCanvas() {
    Mpainter->end();
    delete map;
}

//returns a pointer to the map
QPixmap* PixelCanvas::getMap(){
    return map;
}

//draws a given pixel on the map
void PixelCanvas::drawPixel(int x, int y, QColor color) {

    //create rect set color of brush
    QRect rect(x, y, MpixelViewSize, MpixelViewSize);
    Mpainter->setBrush(QBrush(color));

    //paint with or without grid
    if(Mgrid)Mpainter->drawRect(rect);
    else Mpainter->fillRect(rect, Mpainter->brush());
}

//resized the canvas
void PixelCanvas::resize(int width, int height){
    //creates new map and fills with background
    QPixmap* newMap = new QPixmap(width, height);
    newMap->fill(BACKGROUND_COLOR);

    //set new Mpainter
    delete Mpainter;
    Mpainter = new QPainter(newMap);

    //paints the old map on the new map
    Mpainter->drawPixmap(0, 0, *map);

    //delete old map
    delete map;

    //set new map as the map being used
    map = newMap;
}

//returns pixel size
int PixelCanvas::getPixelViewSize() {
    return MpixelViewSize;
}

//sets pixel size
void PixelCanvas::setPixelViewSize(int newSize) {
    MpixelViewSize = newSize;
}

//moves canvas horizontally by valu pixels
void PixelCanvas::moveHoriz(int val) {
    //creates new map and fills
    QPixmap* newMap = new QPixmap(map->width(), map->height());
    newMap->fill(BACKGROUND_COLOR);

    //deletes old painter and creates new
    delete Mpainter;
    Mpainter = new QPainter(newMap);

    //draw old map on new map offset by val pixels
    Mpainter->drawPixmap(val*MpixelViewSize, 0, *map);

    //delete old map from memory
    delete map;

    //set new map
    map = newMap;
}

//moves canvas vertically by val
void PixelCanvas::moveVert(int val) {
    //create new map and fill with background
    QPixmap* newMap = new QPixmap(map->width(), map->height());
    newMap->fill(BACKGROUND_COLOR);

    //delete old painter and set painter on new map
    delete Mpainter;
    Mpainter = new QPainter(newMap);

    //draw old map on new map offset by val y pixels
    Mpainter->drawPixmap(0, val*MpixelViewSize, *map);

    //delete old map
    delete map;

    //set map to new map
    map = newMap;
}

//paints entire column at x val with given vector of colors
void PixelCanvas::paintColumn(int xVal, std::vector<QColor> pixels) {
    //paint rect for each pixel in vector
    for(int i = 0; i < pixels.size(); i++) {
        Mpainter->setBrush(pixels[i]);
        QRect rect(xVal*MpixelViewSize, i*MpixelViewSize, MpixelViewSize, MpixelViewSize);
        Mpainter->setBrush(QBrush(pixels[i]));

        //paint with or without grid
        if(Mgrid)Mpainter->drawRect(rect);
        else Mpainter->fillRect(rect, Mpainter->brush());
    }
}

//paints entire row at y val with given vector of colors
void PixelCanvas::paintRow(int yVal, std::vector<QColor> pixels) {
    //paint rect for each pixel in vector
    for(int i = 0; i < pixels.size(); i++) {
        QRect rect(i*MpixelViewSize, yVal*MpixelViewSize, MpixelViewSize, MpixelViewSize);
        Mpainter->setBrush(QBrush(pixels[i]));

        //paint with or without grid
        if(Mgrid)Mpainter->drawRect(rect);
        else Mpainter->fillRect(rect, Mpainter->brush());
    }
}

//draws a dotted line rect of selection color at given coords
void PixelCanvas::drawSelectRect(int left, int top, int right, int bot) {
    //change pen to select pen
    QPen pen(SELECTION_BORDER_COLOR);
    pen.setStyle(Qt::DashLine);
    Mpainter->setPen(pen);

    //draw lines of rect
    Mpainter->drawLine(left, top, right, top);
    Mpainter->drawLine(right, top, right, bot);
    Mpainter->drawLine(right, bot, left, bot);
    Mpainter->drawLine(left, bot, left, top);

    //change pen back to normal pen
    pen.setColor(PIXEL_BORDER_COLOR);
    pen.setStyle(Qt::SolidLine);
    Mpainter->setPen(pen);
}

//turns grid on or off
void PixelCanvas::toggleGrid(bool on) {
    Mgrid = on;
}

//clears canvas to background color
void PixelCanvas::clear() {
    map->fill(BACKGROUND_COLOR);
}
