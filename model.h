//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

//Header file for model; declares member variables, slots, and signals

#ifndef MODEL_H
#define MODEL_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QTime>
#include <math.h>
#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QMessageBox>
#include <iostream>
#include <string>
#include <QDebug>
#include <algorithm>

#include "sprite.h"
#include "viewport.h"
#include "pixelcanvas.h"
#include "QString"

class Model : public QObject
{
    Q_OBJECT

//Member variables
private:
    int McanvasWidth;
    int McanvasHeight;
    bool Mselecting;
    int MselectingRectLeft;
    int MselectingRectTop;
    int MselectingRectRight;
    int MselectingRectBot;
    int MhorizPixels;
    int MvertPixels;
    bool MgridOn;
    double MzoomMultiplier;
    int MpreviewSize;
    int McurrentSize;
    int MotherSize;

    //Color values
    QColor MprimaryColor;
    QColor MsecondaryColor;

    QTimer* Mtimer;
    QTime Mtime;
    int MpreviewFrame;
    bool MtimeIgnore;
    double MpreviewSpeed;
    int Mscale;
    int MscaleCurrent;
    int MscaleOther;

    QString MfileName;

    //active was here: int activeSprite;

    //model constant vals
    const int PIXELS_PER_SCROLL = 1;
    const int ZOOM_INC_MULT = 2;

public:
    //Constructor
    explicit Model(QObject *parent = 0);
    ~Model();
    //current active sprite
    int MactiveSprite;\
    //for prompts and checks
    bool MStartedProject;
    bool Mchanged;
    bool MhasBeenSaved;
    bool Mselected;
    //Data structures
    viewport* Mvp;
    viewport* Mselection;
    viewport* MpreviewViewPort;
    PixelCanvas* Mcanvas = nullptr;
    std::vector<Sprite> Msprites;
    std::vector<PixelCanvas*> MpreviewMaps;
    //Returns the active sprite
    Sprite getActiveSprite();

private:
    //helper methods
    //returns x location of rotations point around center
    int rotateX90(int, int, int, int);
    //returns y location of point rotating around center
    int rotateY90(int,int,int,int);
    //repaints the selection outline, left,top,right,bot on canvas
    void paintSelection(int,int,int,int);
    //mirrors selection horizontally over x value of center as arg
    void mirrorHoriz(int);
    //mirrors selection horizontally over x value of center as arg
    void mirrorVert(int);

public slots:
    void createNewFile(int, int);
    void updateCanvasSize(int, int);
    void fillCanvas();
    void changeFrame(int);
    void draw(int, int, QColor, int);
    void erase(int, int, int);

    //For preview updating
    void updatePreview();
    void fillPreviewFromOpenAndNew(int, PixelCanvas*);
    void fillPreviewFromChange(int);
    void fillPreviewFromAdd();

    //Frame alteration
    void fillFramesFromNew(PixelCanvas*, PixelCanvas*);
    void fillFramesFromOpen(int, PixelCanvas*, PixelCanvas*);
    void fillFramesFromAdd(int, PixelCanvas*, PixelCanvas*);
    void fillFramesFromDeleteLast(PixelCanvas*);
    void fillFramesFromDeleteEnd(PixelCanvas*, PixelCanvas*);
    void fillFramesFromDelete(PixelCanvas*, PixelCanvas*, PixelCanvas*);
    void fillFramesFromFillOrPaste();
    void addFrameButton();
    void frameDeleteButton();
    void nextFrame();
    void previousFrame();

    //Preview Speed
    void slowDown();
    void speedUp();
    void pause();
    void play();

    //Scrolling functions
    void scrollCanvasRight();
    void scrollCanvasLeft();
    void scrollCanvasDown();
    void scrollCanvasUp();

    //Prompt for new file
    void changeFalse();
    void projectStarted();
    void beenSaved();

    //selection slots
    void selectionReleased(int, int, int, int);
    void deselect();
    void setSelecting(bool);
    void drawSelecting(int, int, int, int);

    //display grid toggle
    void toggleGrid(bool);

    //Open File
    void fileOpen(std::vector<Sprite>);

    //maybe temporarily
    void pasteFromClipboard();
    void copyToClipboard();

    //zoom slots
    void zoomIn();
    void zoomOut();

    //fill slot
    void fillSelection();

    //color selection slots
    void receivePrimaryColor(QColor);
    void receiveSecondaryColor(QColor);

    //rotate slot integer degree value, boolean if mirrored vert, boolean if mirrored horiz
    void rotateSelection(int, bool, bool);

signals:
    //draws canvas on the screen
    void updateCanvas(QPixmap);
    //adds new frame
    //void newFrame();
    //time fire for preview update
    void timerFire(QPixmap);
    //refresh scroll buttons
    void refreshScrollButtonsSignal();
    //refresh zoom buttons
    void refreshZoomButtonsSignal(double);
    //sends grid value to ui
    void sendGridSwitch(bool);
    //Caraousel Current
    void updateCurrent(QPixmap);
    //Caraousel Previous
    void updatePrevious(QPixmap);
    //Caraousel Next
    void updateNext(QPixmap);

};

#endif // MODEL_H
