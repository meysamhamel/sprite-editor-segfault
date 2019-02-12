//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

//The model holds all of the information that is used on the GUI.

#include "model.h"
#include <QClipboard>
#include <QMimeData>
#include <QImage>
#include <QApplication>
#include <iostream>

Model::Model(QObject *parent) : QObject(parent)
{
    //Integer member variables
    MactiveSprite = 0;
    MpreviewSize = 245;
    McurrentSize = 70;
    MotherSize = 55;
    MpreviewSpeed = 16.6666;
    MselectingRectBot = 0;
    MselectingRectLeft = 0;
    MselectingRectRight = 0;
    MselectingRectTop = 0;
    MzoomMultiplier = 1.0;
    Mscale = 0;
    MscaleCurrent = 0;
    MscaleOther = 0;
    //Boolean member variables
    MtimeIgnore = false;
    MStartedProject = false;
    Mchanged = false;
    MhasBeenSaved = false;
    Mselected = false;
    Mselecting = false;
    MgridOn = true;
    //Object member variables.
    MprimaryColor = Qt::black;
    MsecondaryColor = Qt::white;
    Mtimer = new QTimer(this);
    //Timer fire signal for preview update.
    connect(Mtimer, SIGNAL(timeout()), this, SLOT(updatePreview()));
}

Model::~Model() {
    delete Mvp;
    delete Mselection;
    delete Mcanvas;
    delete Mtimer;
}

//Creates new project with one sprite and size given by user.
void Model::createNewFile(int w, int h)
{
    //Ignore timer to avoid conflict
    MtimeIgnore = true;
    if(MStartedProject == false)
        {
        //Clear data structures and initialize first sprite.
        MpreviewMaps.clear();
        Msprites.clear();
        Sprite s(w, h);
        Msprites.push_back(s);
        MactiveSprite = Msprites.size()-1;
        //Initiates Mcanvas
        Mcanvas = new PixelCanvas(McanvasWidth, McanvasHeight, (int)MzoomMultiplier);
        //initiates viewport
        Mvp = new viewport(0, 0, McanvasWidth/Mcanvas->getPixelViewSize(), McanvasHeight/Mcanvas->getPixelViewSize());
        if(Mvp->getHeight() > s.getHeight()) MvertPixels = s.getHeight();
        else MvertPixels = Mvp->getHeight();
        if(Mvp->getWidth() > s.getWidth()) MhorizPixels = s.getWidth();
        else MhorizPixels = Mvp->getWidth();
        //set Mcanvas grid
        if(!MgridOn) Mcanvas->toggleGrid(false);
        //borders can't be drawn at 1 pixel width, so toggle grid off
        if(MzoomMultiplier <= 1 && MgridOn) toggleGrid(false);
        //This if/else section is to determine scaling for all Qlabels we will be drawing on.
        int size;
        if(Msprites[MactiveSprite].getWidth() > Msprites[MactiveSprite].getHeight())
            {
            size = Msprites[MactiveSprite].getWidth();
            }
        else
            {
            size = Msprites[MactiveSprite].getHeight();
            }
            Mscale = MpreviewSize/size;
            MscaleCurrent = McurrentSize/size;
            MscaleOther = MotherSize/size;
        if(Mscale < 1)
            {
            Mscale = 1;
            }
        if(MscaleCurrent < 1)
            {
            MscaleCurrent = 1;
            }
        if(MscaleOther < 1)
            {
            MscaleOther = 1;
            }
        //Set up first Mcanvas's for the preview and frames and initialize the gui
        PixelCanvas *tempCanvas = new PixelCanvas(MpreviewSize, MpreviewSize, Mscale);
        PixelCanvas *tempCurrent = new PixelCanvas(McurrentSize, McurrentSize, MscaleCurrent);
        PixelCanvas *tempOthers = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
        tempCanvas->toggleGrid(false);
        tempOthers->toggleGrid(false);
        tempCurrent->toggleGrid(false);
        fillPreviewFromOpenAndNew(MactiveSprite, tempCanvas);
        fillFramesFromNew(tempCurrent, tempOthers);
        fillCanvas();
        //initialize Mselection
        Mselection = new viewport(0, 0, Msprites[MactiveSprite].getWidth(), Msprites[MactiveSprite].getHeight());
        //refresh zoom buttons
        emit refreshZoomButtonsSignal(MzoomMultiplier);
        //timer for preview
        Mtimer->start(MpreviewSpeed);
        //set booleans
        MStartedProject = true;
        MhasBeenSaved = false;
        Mchanged = true;
        }
    //Unignore timer to continue preview updating.
    MtimeIgnore = false;
}

//Uses info from already parsed file to load into preview data structures and sprite
//sprite data structures and displays last two frames.
 void Model::fileOpen(std::vector<Sprite> inMsprites)
{
     //Ignore time to avoid conflict
     MtimeIgnore = true;
    if(MStartedProject == false)
        {
        Msprites.clear();
        MpreviewMaps.clear();
        //Initiates Mcanvas
        Mcanvas = new PixelCanvas(McanvasWidth, McanvasHeight, (int)MzoomMultiplier);
        //initiates viewport
        Mvp = new viewport(0, 0, McanvasWidth/Mcanvas->getPixelViewSize(), McanvasHeight/Mcanvas->getPixelViewSize());
        Msprites = inMsprites;
        MactiveSprite = Msprites.size()-1;
        //Calculate scale for preview/frame/canvas pixelview
        int size;
        if(Msprites[MactiveSprite].getWidth() > Msprites[MactiveSprite].getHeight())
            {
            size = Msprites[MactiveSprite].getWidth();
            }
        else
            {
            size = Msprites[MactiveSprite].getHeight();
            }
            Mscale = MpreviewSize/size;
            MscaleCurrent = McurrentSize/size;
            MscaleOther = MotherSize/size;
        if(Mscale < 1)
            {
            Mscale = 1;
            }
        if(MscaleCurrent < 1)
            {
            MscaleCurrent = 1;
            }
        if(MscaleOther < 1)
            {
            MscaleOther = 1;
            }
        //Load the preview canvas's into the data structure
        for(int i = 0; i <= Msprites.size()-1; i++)
            {
            PixelCanvas * tempCanvas = new PixelCanvas(MpreviewSize, MpreviewSize,Mscale);
            tempCanvas->toggleGrid(false);
            fillPreviewFromOpenAndNew(i, tempCanvas);
            }
        //Show last two sprites in frames on current and previous
        PixelCanvas *tempCanvasCurrent = new PixelCanvas(McurrentSize, McurrentSize, MscaleCurrent);
        PixelCanvas *tempCanvasOther = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
        tempCanvasCurrent->toggleGrid(false);
        tempCanvasOther->toggleGrid(false);
        fillFramesFromOpen(Msprites.size()-1, tempCanvasCurrent, tempCanvasOther);
        //set pixel counts
        Sprite s = Msprites[MactiveSprite];
        if(Mvp->getHeight() > s.getHeight()) MvertPixels = s.getHeight();
        else MvertPixels = Mvp->getHeight();
        if(Mvp->getWidth() > s.getWidth()) MhorizPixels = s.getWidth();
        else MhorizPixels = Mvp->getWidth();
        //toggle grid
        //set Mcanvas grid
        if(!MgridOn) Mcanvas->toggleGrid(false);
        //borders can't be drawn at 1 pixel width, so toggle grid off
        if(MzoomMultiplier <= 1 && MgridOn) toggleGrid(false);
        //Fill Mcanvas w/ pixels
        fillCanvas();
        MStartedProject = true;
        Mchanged = false;
        MhasBeenSaved = true;
        //initializes Mselection
        Mselection = new viewport(0, 0, Msprites[MactiveSprite].getWidth(), Msprites[MactiveSprite].getHeight());
        //refresh zoom buttons
        emit refreshZoomButtonsSignal(MzoomMultiplier);
        Mtimer->start(MpreviewSpeed);
        }
    MtimeIgnore = false;
}


//these three methods are used for prompting.
void Model::changeFalse()
{
    Mchanged = false;
}
void Model::projectStarted()
{
    MStartedProject = false;
}
void Model::beenSaved()
{
    MhasBeenSaved = true;
}

//************************MAIN REPAINT METHOD*************************//
//repaints all pixels on canvas and updates ui
void Model::fillCanvas()
{
    //Ignore time to avoid conflicts
    MtimeIgnore = true;
    //Use active sprite
    Sprite s = Msprites[MactiveSprite];
    Mcanvas->clear();
    for (int i = Mvp->getTop(); i < MvertPixels + Mvp->getTop(); i++)
        {
        std::vector<QColor> row;
        for (int j = Mvp->getLeft(); j < MhorizPixels + Mvp->getLeft(); j++)
            {
            row.push_back(s.getPixelColor(j, i));
            }
        Mcanvas->paintRow(i-Mvp->getTop(), row);
        }
    //draw Mselection rectangle
    if(Mselected)
    {
        int pvs = Mcanvas->getPixelViewSize();
        Mcanvas->drawSelectRect((Mselection->getLeft() - Mvp->getLeft())*pvs, (Mselection->getTop()- Mvp->getTop())*pvs,
                               (Mselection->getRight() - Mvp->getLeft())*pvs, (Mselection->getBot() - Mvp->getTop())*pvs);
        MselectingRectBot = 0;
        MselectingRectLeft = 0;
        MselectingRectRight = 0;
        MselectingRectTop = 0;
    }
    emit updateCanvas((*Mcanvas->getMap()));
    MtimeIgnore = false;
}


//change active sprite
void Model::changeFrame(int active) {
    MactiveSprite = active;
    fillCanvas();
    refreshScrollButtonsSignal();
}

void Model::draw(int Xpos, int Ypos, QColor pencolor, int pensize){
    MtimeIgnore = true;
    if(Xpos >= MhorizPixels*Mcanvas->getPixelViewSize() || Ypos >= MvertPixels*Mcanvas->getPixelViewSize()) return;

    int x = Xpos/Mcanvas->getPixelViewSize();
    int y = Ypos/Mcanvas->getPixelViewSize();
    for( int a = 0; a < (pensize); a = a + 1 ) {
        for( int b = 0; b < (pensize); b = b + 1 ) {
            if((y-b) >= 0 && (y-b) < Msprites[MactiveSprite].getHeight()){
                if((x-a) >= 0 && (x+a) < Msprites[MactiveSprite].getWidth()){
                    Msprites[MactiveSprite].changePixelColor((x+a+Mvp->getLeft()), y-b+Mvp->getTop(), pencolor);
                    Mcanvas->drawPixel((Xpos - Xpos % Mcanvas->getPixelViewSize())+a, (Ypos - Ypos % Mcanvas->getPixelViewSize())-b, pencolor); //NEED TO ADD PENSIZE BUT WITH DIFFERENT TECHNIQUE
                    MpreviewMaps[MactiveSprite]->drawPixel((x * MpreviewMaps[MactiveSprite]->getPixelViewSize())+a, y * MpreviewMaps[MactiveSprite]->getPixelViewSize()-b, pencolor);

                    //qDebug() << "====" << "X: " << x << "Y: " << y << "y-b: " << y-b << "x+a: " << x+a << "total pos: " << (y-b)*width+(x+a);
                }
            }
        }
    }

    fillPreviewFromChange(MactiveSprite);
    //Msprites[MactiveSprite].changePixelColor(Xpos/Mcanvas->getPixelViewSize(), Ypos/Mcanvas->getPixelViewSize(), pencolor, pensize);//drawPixel(Xpos,Ypos, pencolor, pensize);
    //Mcanvas->drawPixel(Xpos - Xpos % Mcanvas->getPixelViewSize(), Ypos - Ypos % Mcanvas->getPixelViewSize(), pencolor); //NEED TO ADD PENSIZE BUT WITH DIFFERENT TECHNIQUE
    fillPreviewFromChange(MactiveSprite);
    fillFramesFromFillOrPaste();
    fillCanvas();
    emit updateCanvas(*Mcanvas->getMap());
    emit updateCurrent(*MpreviewMaps[MactiveSprite]->getMap());
    MtimeIgnore = false;
    Mchanged = true;
}

void Model::erase(int Xpos, int Ypos, int pensize) {
    MtimeIgnore = true;
    if(Xpos >= MhorizPixels*Mcanvas->getPixelViewSize() || Ypos >= MvertPixels*Mcanvas->getPixelViewSize()) return;

    int x = Xpos/Mcanvas->getPixelViewSize();
    int y = Ypos/Mcanvas->getPixelViewSize();
    for( int a = 0; a < (pensize); a = a + 1 ) {
        for( int b = 0; b < (pensize); b = b + 1 ) {
            if((y-b) >= 0 && (y-b) < Msprites[MactiveSprite].getHeight()){
                if((x+a) >= 0 && (x+a) < Msprites[MactiveSprite].getWidth()){
                    Msprites[MactiveSprite].changePixelColor((x+a), y-b, MsecondaryColor);
                    Mcanvas->drawPixel((Xpos - Xpos % Mcanvas->getPixelViewSize())+a, (Ypos - Ypos % Mcanvas->getPixelViewSize())-b, MsecondaryColor); //NEED TO ADD PENSIZE BUT WITH DIFFERENT TECHNIQUE
                    MpreviewMaps[MactiveSprite]->drawPixel((x * MpreviewMaps[MactiveSprite]->getPixelViewSize())+a, y * MpreviewMaps[MactiveSprite]->getPixelViewSize()-b, MsecondaryColor);

                    //qDebug() << "====" << "X: " << x << "Y: " << y << "y-b: " << y-b << "x+a: " << x+a << "total pos: " << (y-b)*width+(x+a);
                }
            }
        }
    }

    //Msprites[MactiveSprite].changePixelColor(Xpos/Mcanvas->getPixelViewSize(), Ypos/Mcanvas->getPixelViewSize(), pencolor, pensize);//drawPixel(Xpos,Ypos, pencolor, pensize);
    //Mcanvas->drawPixel(Xpos - Xpos % Mcanvas->getPixelViewSize(), Ypos - Ypos % Mcanvas->getPixelViewSize(), pencolor); //NEED TO ADD PENSIZE BUT WITH DIFFERENT TECHNIQUE
    fillPreviewFromChange(MactiveSprite);
    fillFramesFromFillOrPaste();
    fillCanvas();
    emit updateCanvas(*Mcanvas->getMap());
    emit updateCurrent(*MpreviewMaps[MactiveSprite]->getMap());
    Mchanged = true;
    MtimeIgnore = false;
}

void Model::pasteFromClipboard(){
    MtimeIgnore = true;
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    int x = Mselection->getLeft();
    int y = Mselection->getTop();

    if (mimeData->hasImage()) {
        QImage toBeCopied = qvariant_cast<QImage>(mimeData->imageData());
        Msprites[MactiveSprite].pasteQMapAt(x,y,toBeCopied);
    }
    fillPreviewFromChange(MactiveSprite);
    fillFramesFromFillOrPaste();
    fillCanvas();    
    Mchanged = true;
    MtimeIgnore = false;
}

void Model::copyToClipboard() {
    QClipboard *clipboard = QApplication::clipboard();
    if (!Mselected) {
        Mselection->moveXTo(0);
        Mselection->moveYTo(0);
        Mselection->setHeight(Msprites[MactiveSprite].getHeight());
        Mselection->setWidth(Msprites[MactiveSprite].getWidth());
    }
    QImage CopyRegion{Mselection->getWidth(), Mselection->getHeight(), QImage::Format_ARGB32};
    for (int i = 0; i < Mselection->getHeight(); i++) {
        for (int j = 0; j < Mselection->getWidth(); j++){
            CopyRegion.setPixel(j, i, Msprites[MactiveSprite].getPixelColor(j+Mselection->getLeft(),i + Mselection->getTop()).rgba());
        }
    }

    clipboard->setImage(CopyRegion, QClipboard::Clipboard);
}

//When screen is resized updates the size in model
void Model::updateCanvasSize(int w, int h) {
    //if Mcanvas hasn't been initialized, wait until it has to set width and height
    if(Mcanvas == nullptr) {
        McanvasWidth = w;
        McanvasHeight = h;
    }
    else {
        //Resize Canvas
        Mcanvas->resize(w, h);

        //Resize Viewport
        Mvp->setWidth(w/Mcanvas->getPixelViewSize());
        Mvp->setHeight(h/Mcanvas->getPixelViewSize());

        //Repaint Mcanvas
        if(w>McanvasWidth || h>McanvasHeight) {

            Mvp->moveXTo(0);         //temp fix for scroll/resize bug
            Mvp->moveYTo(0);         //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

            fillCanvas();
        }
        McanvasWidth = w;
        McanvasHeight = h;

        emit updateCanvas(*Mcanvas->getMap());
    }
}


//constantly updates preview
void Model::updatePreview()
{

    if(MtimeIgnore == true)
        {
        return;
        }
    else
        {
        if(MpreviewFrame > MpreviewMaps.size()-1)
            {
            MpreviewFrame = 0;
            emit timerFire(*MpreviewMaps[MpreviewFrame]->getMap());
            MpreviewFrame++;
            }
        else
            {
            emit timerFire(*MpreviewMaps[MpreviewFrame]->getMap());
            MpreviewFrame++;
            }
        }
}


//these next 4 methods are for controlling speed of preview
void Model::speedUp()
{
    if(MStartedProject)
        {
        if(MpreviewSpeed <= 3.6666)
            {
            return;
            }
        else
            {
            MpreviewSpeed-=15;
            Mtimer->start(MpreviewSpeed);
            }
        }
}

void Model::slowDown()
{
    if(MStartedProject)
        {
        if(MpreviewSpeed >= 1000)
            {
            return;
            }
        else
            {
            MpreviewSpeed+=20;
            Mtimer->start(MpreviewSpeed);
            }
        }
}

void Model::pause()
{
    if(MStartedProject)
    {
    MtimeIgnore = true;
    }
}

void Model::play()
{
    if(MStartedProject)
    {
    MtimeIgnore = false;
    }
}

//Method that adds new frame from button push on caraousel
void Model::addFrameButton()
{
    MtimeIgnore = true;
    if(MStartedProject == true)
        {
        Sprite s = Msprites[Msprites.size()-1];
        Msprites.push_back(s);
        MactiveSprite = Msprites.size()-1;
        PixelCanvas *tempCanvasCurrent = new PixelCanvas(McurrentSize, McurrentSize, MscaleCurrent);
        PixelCanvas *tempCanvasPrevious = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
        tempCanvasCurrent->toggleGrid(false);
        tempCanvasPrevious->toggleGrid(false);
        fillPreviewFromAdd();
        fillFramesFromAdd(Msprites.size()-1, tempCanvasCurrent, tempCanvasPrevious);

        Mvp->moveXTo(0);         //temp fix to scrollbug
        Mvp->moveYTo(0);         //^^^^^^^^^^^^^^^^^^^^^

        fillCanvas();
        emit refreshScrollButtonsSignal();
        }

    MtimeIgnore = false;
    Mchanged = true;
}

//delete frame from button
void Model::frameDeleteButton()
{
    MtimeIgnore = true;
    if(MStartedProject == true)
        {
        //Case for very last frame
        if(MactiveSprite == 0 && Msprites.size() == 1)
            {
            Sprite s = Msprites[MactiveSprite];
            s.resetPixels();
            Msprites[MactiveSprite] = s;
            fillPreviewFromChange(MactiveSprite);
            PixelCanvas *tempCanvas = new PixelCanvas(McurrentSize, McurrentSize, MscaleCurrent);
            tempCanvas->toggleGrid(false);
            fillFramesFromDeleteLast(tempCanvas);
            }
        //Case for deleting when at end
        else if(MactiveSprite == Msprites.size()-1)
            {
            PixelCanvas *tempCurrentCanvas = new PixelCanvas(McurrentSize, McurrentSize, MscaleCurrent);
            PixelCanvas *tempPreviousCanvas = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
            tempCurrentCanvas->toggleGrid(false);
            tempPreviousCanvas->toggleGrid(false);
            fillFramesFromDeleteEnd(tempCurrentCanvas, tempPreviousCanvas);
            Msprites.pop_back();
            MpreviewMaps.pop_back();
            MactiveSprite--;
            }
        //Regular delete, has a few cases
        else
            {
            PixelCanvas *tempCurrent = new PixelCanvas(McurrentSize, McurrentSize, MscaleCurrent);
            PixelCanvas *tempNext = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
            PixelCanvas *tempPrevious = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
            tempCurrent->toggleGrid(false);
            tempNext->toggleGrid(false);
            tempPrevious->toggleGrid(false);
            fillFramesFromDelete(tempCurrent, tempNext, tempPrevious);
            Msprites.erase(Msprites.begin()+MactiveSprite);
            MpreviewMaps.erase(MpreviewMaps.begin()+MactiveSprite);

            if(MactiveSprite != 0)
            {
                MactiveSprite--;
            }
            }
        fillCanvas();
        }
    MtimeIgnore = false;
    Mchanged = true;
}

Sprite Model::getActiveSprite() {
    return Msprites[MactiveSprite];
}

//scrolls the canvas to the right
void Model::scrollCanvasRight() {
    Sprite sprite = Msprites[MactiveSprite];

    //move Mcanvas right
    Mcanvas->moveHoriz(0 - PIXELS_PER_SCROLL);

    //paint column
    for(int i = 0; i <= PIXELS_PER_SCROLL; i++) {
        std::vector<QColor> column;
        for(int j = Mvp->getTop(); j < MvertPixels + Mvp->getTop(); j++) {
            column.push_back(sprite.getPixelColor(Mvp->getRight()-1+i, j));
        }

        Mcanvas->paintColumn(Mvp->getWidth() - 1 + i, column);
    }

    //updates canvas in ui
    emit updateCanvas(*Mcanvas->getMap());

    //change viewport coords
    Mvp->moveHoriz(PIXELS_PER_SCROLL);

    //sends signal to enable or disable scroll buttons
    emit refreshScrollButtonsSignal();
}

//scrolls the canvas to the left
void Model::scrollCanvasLeft() {
    Sprite sprite = Msprites[MactiveSprite];

    //move Mcanvas right
    Mcanvas->moveHoriz(PIXELS_PER_SCROLL);

    //paint column
    for(int i = 0; i <= PIXELS_PER_SCROLL; i++) {
        std::vector<QColor> column;
        for(int j = Mvp->getTop(); j < MvertPixels + Mvp->getTop(); j++) {
            column.push_back(sprite.getPixelColor(Mvp->getLeft()-1+i, j));
        }

        Mcanvas->paintColumn(i, column);
    }

    //updates canvas in ui
    emit updateCanvas(*Mcanvas->getMap());

    //change viewport coords
    Mvp->moveHoriz(0 - PIXELS_PER_SCROLL);

    //sends signal to enable or disable scroll buttons
    emit refreshScrollButtonsSignal();
}

//scrolls the canvas down
void Model::scrollCanvasDown() {
    Sprite sprite = Msprites[MactiveSprite];

    //move Mcanvas right
    Mcanvas->moveVert(0 - PIXELS_PER_SCROLL);

    //paint column
    for(int i = 0; i <= PIXELS_PER_SCROLL; i++) {
        std::vector<QColor> row;
        for(int j = Mvp->getLeft(); j < MhorizPixels + Mvp->getLeft(); j++) {
            row.push_back(sprite.getPixelColor(j, Mvp->getBot() - 1 + i));
        }

        Mcanvas->paintRow(Mvp->getHeight() - 1 + i, row);
    }

    //updates canvas in ui
    emit updateCanvas(*Mcanvas->getMap());

    //change viewport coords
    Mvp->moveVert(PIXELS_PER_SCROLL);

    //sends signal to enable or disable scroll buttons
    emit refreshScrollButtonsSignal();
}

//scrolls the canvas up
void Model::scrollCanvasUp() {
    Sprite sprite = Msprites[MactiveSprite];

    //move Mcanvas right
    Mcanvas->moveVert(PIXELS_PER_SCROLL);

    //paint column
    for(int i = 0; i <= PIXELS_PER_SCROLL; i++) {
        std::vector<QColor> row;
        for(int j = Mvp->getLeft(); j < MhorizPixels + Mvp->getLeft(); j++) {
            row.push_back(sprite.getPixelColor(j, Mvp->getTop() - 1 + i));
        }

        Mcanvas->paintRow(i, row);
    }

    //updates canvas in ui
    emit updateCanvas(*Mcanvas->getMap());

    //change viewport coords
    Mvp->moveVert(0 - PIXELS_PER_SCROLL);

    //sends signal to enable or disable scroll buttons
    emit refreshScrollButtonsSignal();
}

//sets selection
void Model::selectionReleased(int selLeft, int selTop, int selRight, int selBot) {
    //do nothing if no sprite opened yet
    if(Msprites.size() == 0) return;

    //checks if zero size selection
    if(selLeft == selRight) return;
    if(selTop == selBot) return;

    //checks if Mselection drawn in reverse
    if(selLeft>selRight) {
        int swap = selLeft;
        selLeft = selRight;
        selRight = swap;
    }
    if(selTop>selBot) {
        int swap = selTop;
        selTop = selBot;
        selBot = swap;
    }

    //rounds Mselection to full pixels
    int pvs = Mcanvas->getPixelViewSize();
    selLeft = (selLeft / pvs) + Mvp->getLeft();
    selTop = (selTop / pvs) + Mvp->getTop();
    selRight = (selRight / pvs)+ 1 + Mvp->getLeft();
    selBot = (selBot / pvs) + 1 + Mvp->getTop();

    //sets Mselection
    Mselection->moveXTo(selLeft);
    Mselection->moveYTo(selTop);
    Mselection->setWidth(selRight - selLeft);
    Mselection->setHeight(selBot - selTop);

    //checks if Mselection is out of range
    if(Mselection->getRight() > Mvp->getLeft() + MhorizPixels) Mselection->setWidth(MhorizPixels - Mselection->getLeft());
    if(Mselection->getBot() > Mvp->getTop() + MvertPixels) Mselection->setHeight(MvertPixels - Mselection->getTop());
    if(Mselection->getLeft() < Mvp->getLeft()) {
        Mselection->setWidth(Mselection->getRight() - Mvp->getLeft());
        Mselection->moveXTo(Mvp->getLeft());
    }
    if(Mselection->getTop() < Mvp->getTop()) {
        Mselection->setHeight(Mselection->getBot() - Mvp->getTop());
        Mselection->moveYTo(Mvp->getTop());
    }


    //deselect if top left is out of range or if Mselection is 1 pixel
    if(Mselection->getLeft() > Mvp->getLeft() + MhorizPixels) Mselected = false;
    else if (Mselection->getTop() > Mvp->getTop() + MvertPixels) Mselected = false;
    else if (Mselection->getHeight() == 1 && Mselection->getWidth() == 1) Mselected = false;
    else Mselected = true;

    //refill canvas
    fillCanvas();
}

//unsets selection, paints over selection rect
void Model::deselect() {
    //do nothing if no sprite open yet
    if(Msprites.size() == 0) return;
    if(!Mselected) return;

    //toggle selected variable
    Mselected = false;

    //repaint over selection
    int pvs = Mcanvas->getPixelViewSize();
    paintSelection(Mselection->getLeft() * pvs, Mselection->getTop() * pvs, Mselection->getRight() * pvs, Mselection->getBot() * pvs);
    emit updateCanvas(*Mcanvas->getMap());

    //reset Mselection to entire frame
    Mselection->moveXTo(0);
    Mselection->moveYTo(0);
    Mselection->setWidth(Msprites[MactiveSprite].getWidth());
    Mselection->setHeight(Msprites[MactiveSprite].getHeight());
}

//sets model selecting var
void Model::setSelecting(bool state) {
    Mselecting = state;
}

//toggles the grid on or off and repaints
void Model::toggleGrid(bool on){
    MgridOn = on;

    //only repaints if a file is open
    if(Mcanvas != nullptr)
    {
        Mcanvas->toggleGrid(on);
        fillCanvas();
    }
}

//zooms in on canvas
void Model::zoomIn(){
    //shuts off grid if zoomed in 1x
    if (MzoomMultiplier == 1) {
        MgridOn = true;
        Mcanvas->toggleGrid(true);
        emit sendGridSwitch(true);
    }
    //zoomz in by rate of zoom increment constant
    MzoomMultiplier *= ZOOM_INC_MULT;

    //changes the size of each pixel to the size of the zoom multiplier
    Mcanvas->setPixelViewSize((int)MzoomMultiplier);

    //moves viewport back to default top left of sprite
    Mvp->moveXTo(0);
    Mvp->moveYTo(0);

    //change viewport size
    Mvp->setWidth(McanvasWidth/Mcanvas->getPixelViewSize());
    Mvp->setHeight(McanvasHeight/Mcanvas->getPixelViewSize());

    //change number of pixels member vals
    if (Mvp->getWidth() < Msprites[MactiveSprite].getWidth()) {
        MhorizPixels = Mvp->getWidth();
    }
    else MhorizPixels = Msprites[MactiveSprite].getWidth();
    if (Mvp->getHeight() < Msprites[MactiveSprite].getHeight()) {
        MvertPixels = Mvp->getHeight();
    }\
    else MvertPixels = Msprites[MactiveSprite].getHeight();

    //sends enable or disable button signals
    emit refreshZoomButtonsSignal(MzoomMultiplier);
    emit refreshScrollButtonsSignal();

    //repaints canvas
    fillCanvas();
}

//zooms out on Mcanvas
void Model::zoomOut() {
    MzoomMultiplier /= ZOOM_INC_MULT;

    //turn grid on automatically when zoomed out from min grid val
    if (MzoomMultiplier == 1) {
        MgridOn = false;
        Mcanvas->toggleGrid(false);
        emit sendGridSwitch(false);
    }

    //changes the size of each pixel to the size of the zoom multiplier
    Mcanvas->setPixelViewSize((int)MzoomMultiplier);

    //moves viewport back to default top left of sprite
    Mvp->moveXTo(0);
    Mvp->moveYTo(0);

    //change viewport size
    Mvp->setWidth(McanvasWidth/Mcanvas->getPixelViewSize());
    Mvp->setHeight(McanvasHeight/Mcanvas->getPixelViewSize());

    //change number of pixels member vals
    if (Mvp->getWidth() < Msprites[MactiveSprite].getWidth()) {
        MhorizPixels = Mvp->getWidth();
    }
    else MhorizPixels = Msprites[MactiveSprite].getWidth();
    if (Mvp->getHeight() < Msprites[MactiveSprite].getHeight()) {
        MvertPixels = Mvp->getHeight();
    }
    else MvertPixels = Msprites[MactiveSprite].getHeight();

    //sends enable or disable button signals
    emit refreshZoomButtonsSignal(MzoomMultiplier);
    emit refreshScrollButtonsSignal();

    //repaints canvas
    fillCanvas();
}

//paints all pixels in selection the primary color
void Model::fillSelection() {
    if(MStartedProject == true){
        //ignores preview while filling
        MtimeIgnore = true;

        //change color of all pixels in selection
        for(int i = Mselection->getLeft(); i < Mselection->getRight(); i++) {
            for(int j = Mselection->getTop(); j < Mselection->getBot(); j++) {
                Msprites[MactiveSprite].changePixelColor(i, j, MprimaryColor);
            }
        }

        //update in preview and carousel
        fillPreviewFromChange(MactiveSprite);
        fillFramesFromFillOrPaste();

        //repaint canvas
        fillCanvas();

        //file changed
        Mchanged = true;

        //continue preview
        MtimeIgnore = false;
    }else{
        //Can't call the message because we're calling from the model
        //Next time don't just call it with a signal, but actually bind it to the button on the UI
    }

}

//receives primary color when changed
void Model::receivePrimaryColor(QColor c) {
    MprimaryColor = c;
}

//receives secondary color when changed
void Model::receiveSecondaryColor(QColor c) {
    MsecondaryColor = c;
}

//rotates and mirrors selection around center of selection
void Model::rotateSelection(int degrees, bool vertMirror, bool horizMirror) {
    //ignore preview while editing
    MtimeIgnore = true;

    //create temp sprite of same size
    Sprite temp(Msprites[MactiveSprite].getWidth(), Msprites[MactiveSprite].getHeight());
    temp.fill(MsecondaryColor);

    //find center
    int centerX = (Mselection->getLeft() + Mselection->getRight())/2;
    int centerY = (Mselection->getTop() + Mselection->getBot())/2;

    //call necessary mirror helper functions first
    if(vertMirror) mirrorVert(centerY);
    if(horizMirror) mirrorHoriz(centerX);

    //new bounds stay if rotation is zero deg
    int newSelectionLeft = Mselection->getLeft();
    int newSelectionRight = Mselection->getRight();
    int newSelectionTop = Mselection->getTop();
    int newSelectionBot = Mselection->getBot();

    //rotate bounds after 90
    if(degrees > 0) {
        newSelectionLeft = centerX - (Mselection->getBot() - centerY);
        newSelectionRight = centerX - (Mselection->getTop() - centerY);
        newSelectionTop = centerY - (centerX - Mselection->getLeft());
        newSelectionBot = centerY - (centerX - Mselection->getRight());
    }

    //repeat for 180 or 270
    for (int i = 90; i < degrees; i += 90) {
        int nextSelectionLeft = centerX - (newSelectionBot - centerY);
        int nextSelectionRight = centerX - (newSelectionTop - centerY);
        int nextSelectionTop = centerY - (centerX - newSelectionLeft);
        int nextSelectionBot = centerY - (centerX - newSelectionRight);
        newSelectionLeft = nextSelectionLeft;
        newSelectionRight = nextSelectionRight;
        newSelectionTop = nextSelectionTop;
        newSelectionBot = nextSelectionBot;
    }

    //skip if zero deg rotation
    if(degrees > 0) {
        //rotate into new sprite and erase old Mselection
        for(int i = Mselection->getLeft(); i < Mselection->getRight(); i++) {
            for(int j = Mselection->getTop(); j<Mselection->getBot(); j++) {
                int newX = rotateX90(i, j, centerX, centerY);
                int newY = rotateY90(i, j, centerX, centerY);
                //repeat for 180 or 270
                for (int k = 90; k < degrees; k += 90) {
                    int nextNewX = rotateX90(newX, newY, centerX, centerY);
                    int nextNewY = rotateY90(newX, newY, centerX, centerY);
                    newX = nextNewX;
                    newY = nextNewY;
                }
                if(newX < temp.getWidth() && newX > 0 && newY < temp.getHeight() && newY > 0)
                temp.changePixelColor(newX, newY, Msprites[MactiveSprite].getPixelColor(i, j));
                Msprites[MactiveSprite].changePixelColor(i, j, MsecondaryColor);
            }
        }

        //draw new Mselection in sprite
        for(int i = newSelectionLeft; i < newSelectionRight; i++) {
            for (int j = newSelectionTop; j< newSelectionBot; j++) {
                if((i < Msprites[MactiveSprite].getWidth()) && (j < Msprites[MactiveSprite].getHeight()) && (i > 0) && (j > 0))
                    Msprites[MactiveSprite].changePixelColor(i, j, temp.getPixelColor(i, j));
            }
        }
    }

    //update preview and carousel
    fillPreviewFromChange(MactiveSprite);
    fillFramesFromFillOrPaste();

    //check selection bounds
    if(newSelectionLeft < 0) newSelectionLeft = 0;
    if(newSelectionTop < 0) newSelectionTop = 0;
    if(newSelectionRight > Msprites[MactiveSprite].getWidth()) newSelectionRight = Msprites[MactiveSprite].getWidth();
    if(newSelectionBot > Msprites[MactiveSprite].getHeight()) newSelectionBot = Msprites[MactiveSprite].getHeight();


    //move selection
    Mselection->moveXTo(newSelectionLeft);
    Mselection->moveYTo(newSelectionTop);
    Mselection->setWidth(newSelectionRight - newSelectionLeft);
    Mselection->setHeight(newSelectionBot - newSelectionTop);

    //repaint canvas
    fillCanvas();

    //contine preview
    MtimeIgnore = false;

    //file changed
    Mchanged = true;
}

//helper method to mirror selection horizontally over center
void Model::mirrorHoriz(int centerX) {
    int left = Mselection->getLeft() - 1;
    if (left < 0) left = 0;

    //flips each pixel with its counterpart across center X
    for(int i = left; i < centerX; i++) {
        for(int j = Mselection->getTop(); j < Mselection->getBot(); j++) {
            int deltaX = centerX - i;
            QColor swapColor = Msprites[MactiveSprite].getPixelColor(i, j);
            Msprites[MactiveSprite].changePixelColor(i, j, Msprites[MactiveSprite].getPixelColor(i + 2*deltaX, j));
            Msprites[MactiveSprite].changePixelColor(i + 2*deltaX, j, swapColor);
        }
    }
}

//helper method to mirror selection vertically over center
void Model::mirrorVert(int centerY) {
    int top = Mselection->getTop() - 1;
    if (top < 0) top = 0;

    //flips each pixel with its counterpart across center Y
    for (int i = Mselection->getLeft(); i < Mselection->getRight(); i++) {
        for ( int j = top; j < centerY; j++) {
            int deltaY = centerY - j;
            QColor swapColor = Msprites[MactiveSprite].getPixelColor(i, j);
            Msprites[MactiveSprite].changePixelColor(i, j, Msprites[MactiveSprite].getPixelColor(i, j + 2*deltaY));
            Msprites[MactiveSprite].changePixelColor(i, j + 2*deltaY, swapColor);
        }
    }
}

//returns x value of x,y point rotated around centerx,y
int Model::rotateX90(int x, int y, int centerX, int centerY) {
    int deltaY = centerY - y;
    return centerX + deltaY;
}

//returns y vcalue of x,y point rotated around centerx,y
int Model::rotateY90(int x, int y, int centerX, int centerY) {
    int deltaX = centerX - x;
    return centerY - deltaX;
}

//paints over previous selecting box and draws new one
void Model::drawSelecting(int left, int top, int right, int bot) {
    //do nothing if no sprite open
    if(Msprites.size() == 0) return;

    //paint over previous Mselection
    paintSelection(MselectingRectLeft, MselectingRectTop, MselectingRectRight, MselectingRectBot);

    //inverts if selection is being made in reverse
    if(left > right) {
        int swap = left;
        left = right;
        right = swap;
    }
    if(top > bot) {
        int swap = top;
        top = bot;
        bot = swap;
    }

    //bounds selection to pixels drawn on screen
    if(left < 0) left = 0;
    if(top < 0) top = 0;
    if(bot >= MvertPixels * Mcanvas->getPixelViewSize()) bot = MvertPixels * Mcanvas->getPixelViewSize()-1;
    if(right >= MhorizPixels * Mcanvas->getPixelViewSize()) right = MhorizPixels * Mcanvas->getPixelViewSize()-1;

    //set coords of the Mselecting rect
    MselectingRectLeft = left;
    MselectingRectTop = top;
    MselectingRectRight = right;
    MselectingRectBot = bot;

    //repaint
    Mcanvas->drawSelectRect(MselectingRectLeft, MselectingRectTop, MselectingRectRight, MselectingRectBot);
    emit updateCanvas(*Mcanvas->getMap());
}

//helper method to paint over previous selection
void Model::paintSelection(int left, int top, int right, int bot) {
    //Can be optimized further if paintRow/Col methods are given a starting point

    //convert to coord on sprite
    left = (left / Mcanvas->getPixelViewSize());
    top = (top / Mcanvas->getPixelViewSize());
    right = (right / Mcanvas->getPixelViewSize());
    bot = (bot / Mcanvas->getPixelViewSize());

    //paint top row.
    std::vector<QColor> set;
    for (int i = Mvp->getLeft(); i < Mvp->getLeft()+MhorizPixels; i++) {
        set.push_back(Msprites[MactiveSprite].getPixelColor(i, top + Mvp->getTop()));
    }
    Mcanvas->paintRow(top, set);
    set.clear();

    //paint bot row.
    for (int i = Mvp->getLeft(); i < Mvp->getLeft()+MhorizPixels; i++) {
        //if(i = Mvp->getRight()) break;
        set.push_back(Msprites[MactiveSprite].getPixelColor(i, bot + Mvp->getTop()));
    }
    Mcanvas->paintRow(bot, set);
    set.clear();

    //paint left col
    for (int i = Mvp->getTop(); i < Mvp->getTop() + MvertPixels; i++) {
        set.push_back(Msprites[MactiveSprite].getPixelColor(left + Mvp->getLeft(), i));
    }
    Mcanvas->paintColumn(left, set);
    set.clear();

    //paint right col
    for (int i = Mvp->getTop(); i < Mvp->getTop() + MvertPixels; i++) {
        set.push_back(Msprites[MactiveSprite].getPixelColor(right + Mvp->getLeft(), i));
    }
    Mcanvas->paintColumn(right, set);
    set.clear();
}

//**********************************GIGANTIC SECTION FOR FRAME CARAOUSEL AND PREVIEW!!***********************
//***********************************************************************************************************

//This is for loading initial canvas's into preview data structure
void Model::fillPreviewFromOpenAndNew(int spriteNumber, PixelCanvas * previewCanvas)
{
    MtimeIgnore = true;
    std::vector<QColor> row;
    //Store canvas with sprite in preview data structure
    Sprite previewSprite = Msprites[spriteNumber];
    for(int i = 0; i < previewSprite.getHeight(); i++)
        {
        for(int j = 0; j < previewSprite.getWidth(); j++)
            {
            row.push_back(previewSprite.getPixelColor(j,i));
            }
        previewCanvas->paintRow(i,row);
        row.clear();
        }
    MpreviewMaps.push_back(previewCanvas);
    MtimeIgnore = false;
}

//Anytime paint/fill/rotate/paste are done, update preview data structure data
void Model::fillPreviewFromChange(int previewSprite)
{
    MtimeIgnore = true;
    PixelCanvas *tempCanvas = new PixelCanvas(MpreviewSize, MpreviewSize, Mscale);
    tempCanvas->toggleGrid(false);
    std::vector<QColor> row;
    Sprite s = Msprites[previewSprite];
    //take out pixels and paint them on canvas
    for(int i = 0; i < s.getHeight(); i++)
        {
        for(int j = 0; j < s.getWidth(); j++)
            {
            row.push_back(s.getPixelColor(j,i));
            }

        tempCanvas->paintRow(i,row);
        row.clear();
        }
    MpreviewMaps[previewSprite] = tempCanvas;
    MtimeIgnore = false;
}

//push copy of last canvas on data structure
void Model::fillPreviewFromAdd()
{
    MtimeIgnore = true;
    MpreviewMaps.push_back(MpreviewMaps[MactiveSprite-1]);
    MtimeIgnore = false;
}

//Fill the current and previous frame to display last two sprites
void Model::fillFramesFromOpen(int spriteNumber, PixelCanvas *current, PixelCanvas *previous)
{
    MtimeIgnore = true;
    std::vector<QColor> row;
    Sprite frameSprite = Msprites[spriteNumber];
    //update current frame with current sprite
    for(int i = 0; i < frameSprite.getHeight(); i++)
        {
        for(int j = 0; j < frameSprite.getWidth(); j++)
            {
            row.push_back(frameSprite.getPixelColor(j,i));
            }
        current->paintRow(i,row);
        row.clear();
        }
    //For files larger than 1 sprite
    if(Msprites.size() > 1)
        {
        frameSprite = Msprites[spriteNumber-1];
        //update previous sprite
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            previous->paintRow(i,row);
            row.clear();
            }
        PixelCanvas *tempCanvas = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
        QPixmap *tempMap = tempCanvas->getMap();
        tempMap->fill(Qt::transparent);
        emit updateNext(*tempCanvas->getMap());
        emit updatePrevious(*previous->getMap());
        emit updateCurrent(*current->getMap());
        }
    //Case where there is only 1 sprite
    else
        {
        PixelCanvas *tempNext = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
        PixelCanvas *tempPrevious = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
        QPixmap *tempMapNext = tempNext->getMap();
        QPixmap *tempMapPrevious = tempPrevious->getMap();
        tempMapPrevious->fill(Qt::transparent);
        tempMapNext->fill(Qt::transparent);
        emit updateNext(*tempNext->getMap());
        emit updatePrevious(*previous->getMap());
        emit updateCurrent(*current->getMap());
        }
    MtimeIgnore = false;
}

//Update current and previous frames, as well as empty next frame.
void Model::fillFramesFromAdd(int spriteNumber, PixelCanvas *currentCanvas, PixelCanvas *previousCanvas)
{
    MtimeIgnore = true;
    std::vector<QColor> row;
    Sprite frameSprite = Msprites[spriteNumber];
    //update current frame
    for(int i = 0; i < frameSprite.getHeight(); i++)
        {
        for(int j = 0; j < frameSprite.getWidth(); j++)
            {
            row.push_back(frameSprite.getPixelColor(j,i));
            }
        currentCanvas->paintRow(i,row);
        row.clear();
        }
    frameSprite = Msprites[spriteNumber-1];
    //update previous frame
    for(int i = 0; i < frameSprite.getHeight(); i++)
        {
        for(int j = 0; j < frameSprite.getWidth(); j++)
            {
            row.push_back(frameSprite.getPixelColor(j,i));
            }
        previousCanvas->paintRow(i,row);
        row.clear();
        }
    //make last canvas transparent
    PixelCanvas *tempCanvas = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
    QPixmap *tempMap = tempCanvas->getMap();
    tempMap->fill(Qt::transparent);
    emit updateNext(*tempCanvas->getMap());
    emit updateCurrent(*currentCanvas->getMap());
    emit updatePrevious(*previousCanvas->getMap());
    MtimeIgnore = false;
}

//Last frame left is set to a white canvas
void Model::fillFramesFromDeleteLast(PixelCanvas *newCanvas)
{
    MtimeIgnore = true;
    std::vector<QColor> row;
    Sprite frameSprite = Msprites[MactiveSprite];
    for(int i = 0; i < frameSprite.getHeight(); i++)
        {
        for(int j = 0; j < frameSprite.getWidth(); j++)
            {
            row.push_back(frameSprite.getPixelColor(j,i));
            }
        newCanvas->paintRow(i,row);
        row.clear();
        }
    emit updateCurrent(*newCanvas->getMap());
    MtimeIgnore = false;
}

//Deleting frames when at the end of the frames
void Model::fillFramesFromDeleteEnd(PixelCanvas *currentCanvas, PixelCanvas *previousCanvas)
{
    MtimeIgnore = true;
    std::vector<QColor> row;
    Sprite frameSprite = Msprites[MactiveSprite-1];
   for(int i = 0; i < frameSprite.getHeight(); i++)
        {
        for(int j = 0; j < frameSprite.getWidth(); j++)
            {
            row.push_back(frameSprite.getPixelColor(j,i));
            }
        currentCanvas->paintRow(i,row);
        row.clear();
        }
   //If previous sprite is first sprite in data structure, make sure to clear it.
    if(MactiveSprite-1 == 0)
        {
        QPixmap *tempMap = previousCanvas->getMap();
        tempMap->fill(Qt::transparent);
        }
   //update previous frame with one behind it.
    else
        {
        Sprite frameSprite = Msprites[MactiveSprite-2];
        for(int i = 0; i< frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            previousCanvas->paintRow(i,row);
            row.clear();
            }
        }
    emit updatePrevious(*previousCanvas->getMap());
    emit updateCurrent(*currentCanvas->getMap());
    MtimeIgnore = false;
}

//General delete, has multiple cases
void Model::fillFramesFromDelete(PixelCanvas* current, PixelCanvas* next, PixelCanvas* previous)
{
    MtimeIgnore = true;
    std::vector<QColor> row;
    Sprite frameSprite = Msprites[MactiveSprite];
    //Need case for very front and last on right
    if(MactiveSprite+1 == Msprites.size()-1 && MactiveSprite == 0)
        {
        frameSprite = Msprites[MactiveSprite+1];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            current->paintRow(i,row);
            row.clear();
            }
        QPixmap *tempMap = next->getMap();
        tempMap->fill(Qt::transparent);
        emit updateCurrent(*current->getMap());
        emit updateNext(*next->getMap());
        }
    //Case for very front and more on right
    else if(MactiveSprite+1 != Msprites.size()-1 && MactiveSprite == 0)
        {
            frameSprite = Msprites[MactiveSprite+1];
            for(int i = 0; i < frameSprite.getHeight(); i++)
                {
                for(int j = 0; j < frameSprite.getWidth(); j++)
                    {
                    row.push_back(frameSprite.getPixelColor(j,i));
                    }
                current->paintRow(i,row);
                row.clear();
                }
            frameSprite = Msprites[MactiveSprite+2];
            for(int i = 0; i < frameSprite.getHeight(); i++)
                {
                for(int j = 0; j < frameSprite.getWidth(); j++)
                    {
                    row.push_back(frameSprite.getPixelColor(j,i));
                    }
                next->paintRow(i,row);
                row.clear();
                }
            emit updateCurrent(*current->getMap());
            emit updateNext(*next->getMap());
        }
    //Need case for middle and left is last
    else if(MactiveSprite-1 == 0)
        {
        frameSprite = Msprites[MactiveSprite-1];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            current->paintRow(i,row);
            row.clear();
            }
        QPixmap *tempMap = previous->getMap();
        tempMap->fill(Qt::darkGray);
        emit updateCurrent(*current->getMap());
        emit updatePrevious(*previous->getMap());
        }
    //need case for deleting middle and left moves over
    else
        {
        frameSprite = Msprites[MactiveSprite-1];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            current->paintRow(i,row);
            row.clear();
            }
        frameSprite = Msprites[MactiveSprite-2];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            previous->paintRow(i,row);
            row.clear();
            }
        emit updateCurrent(*current->getMap());
        emit updatePrevious(*previous->getMap());
        }
    MtimeIgnore = false;
}

//updates all frames for slide from left to right on frame caraousel
void Model::nextFrame()
{
    MtimeIgnore = true;
    std::vector<QColor> row;
    Sprite frameSprite = Msprites[MactiveSprite];
    //Initiate some canvas's
    PixelCanvas *tempCurrent = new PixelCanvas(McurrentSize, McurrentSize, MscaleCurrent);
    PixelCanvas *tempNext = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
    PixelCanvas *tempPrevious = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
    tempCurrent->toggleGrid(false);
    tempNext->toggleGrid(false);
    tempPrevious->toggleGrid(false);
    //Case for if there is no more frames to right
    if(MactiveSprite == Msprites.size()-1)
        {
        return;
        }
    //case for if the next one on right is last one OR the left is empty and right will be empty.
    else if(MactiveSprite+1 == Msprites.size()-1 || (MactiveSprite == 0 && MactiveSprite+1 == Msprites.size()-1))
        {
        frameSprite = Msprites[MactiveSprite+1];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            tempCurrent->paintRow(i,row);
            row.clear();
            }
        frameSprite = Msprites[MactiveSprite];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            tempPrevious->paintRow(i,row);
            row.clear();
            }
        QPixmap *tempMap = tempNext->getMap();
        tempMap->fill(Qt::transparent);
        emit updateNext(*tempNext->getMap());
        emit updateCurrent(*tempCurrent->getMap());
        emit updatePrevious(*tempPrevious->getMap());
        changeFrame(MactiveSprite+1);
        }
    //Regular delete
    else
    {
        frameSprite = Msprites[MactiveSprite+2];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            tempNext->paintRow(i,row);
            row.clear();
            }
        frameSprite = Msprites[MactiveSprite+1];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            tempCurrent->paintRow(i,row);
            row.clear();
            }
        frameSprite = Msprites[MactiveSprite];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            tempPrevious->paintRow(i,row);
            row.clear();
            }
        emit updateNext(*tempNext->getMap());
        emit updateCurrent(*tempCurrent->getMap());
        emit updatePrevious(*tempPrevious->getMap());
        changeFrame(MactiveSprite+1);
    }
    MtimeIgnore = false;
}

//Updates all canvas's for slide from left to right
void Model::previousFrame()
{
    MtimeIgnore = true;
    std::vector<QColor> row;
    Sprite frameSprite = Msprites[MactiveSprite];
    PixelCanvas *tempCurrent = new PixelCanvas(McurrentSize, McurrentSize, MscaleCurrent);
    PixelCanvas *tempNext = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
    PixelCanvas *tempPrevious = new PixelCanvas(MotherSize, MotherSize, MscaleOther);
    tempCurrent->toggleGrid(false);
    tempNext->toggleGrid(false);
    tempPrevious->toggleGrid(false);
    //case for if there is no more frames to left
    if(MactiveSprite == 0)
        {
            return;
        }
    //case for if the next one to left is last one OR the right one is empty and the left one will be empty.
    else if(MactiveSprite-1 == 0)
        {
        frameSprite = Msprites[MactiveSprite-1];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            tempCurrent->paintRow(i,row);
            row.clear();
        }
        frameSprite = Msprites[MactiveSprite];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            tempNext->paintRow(i,row);
            row.clear();
            }
        QPixmap *tempMap = tempPrevious->getMap();
        tempMap->fill(Qt::transparent);
        emit updateCurrent(*tempCurrent->getMap());
        emit updatePrevious(*tempPrevious->getMap());
        emit updateNext(*tempNext->getMap());
        changeFrame(MactiveSprite-1);
        }
    //case for normal previousframe, update all 3 frames
    else
        {
        frameSprite = Msprites[MactiveSprite-2];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            tempPrevious->paintRow(i,row);
            row.clear();
            }
        frameSprite = Msprites[MactiveSprite-1];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            tempCurrent->paintRow(i,row);
            row.clear();
            }
        frameSprite = Msprites[MactiveSprite];
        for(int i = 0; i < frameSprite.getHeight(); i++)
            {
            for(int j = 0; j < frameSprite.getWidth(); j++)
                {
                row.push_back(frameSprite.getPixelColor(j,i));
                }
            tempNext->paintRow(i,row);
            row.clear();
            }
        emit updateNext(*tempNext->getMap());
        emit updateCurrent(*tempCurrent->getMap());
        emit updatePrevious(*tempPrevious->getMap());
        changeFrame(MactiveSprite-1);
    }
    MtimeIgnore = false;
}

//Fill current frame with white and the next and previous frames set to transparent
void Model::fillFramesFromNew(PixelCanvas* current, PixelCanvas* others)
{
    MtimeIgnore = true;
    std::vector<QColor> row;
    Sprite frameSprite = Msprites[MactiveSprite];
    for(int i = 0; i < frameSprite.getHeight(); i++)
        {
        for(int j = 0; j < frameSprite.getWidth(); j++)
            {
            row.push_back(frameSprite.getPixelColor(j,i));
            }
        current->paintRow(i,row);
        row.clear();
        }
    QPixmap *tempMap = others->getMap();
    tempMap->fill(Qt::transparent);
    emit updateCurrent(*current->getMap());
    emit updatePrevious(*others->getMap());
    emit updateNext(*others->getMap());
    MtimeIgnore = false;
}

//Update current canvas's after any altering
void Model::fillFramesFromFillOrPaste()
{
    MtimeIgnore = true;
    std::vector<QColor> row;
    Sprite frameSprite = Msprites[MactiveSprite];
    PixelCanvas *tempCurrent = new PixelCanvas(McurrentSize, McurrentSize, MscaleCurrent);
    tempCurrent->toggleGrid(false);
    for(int i = 0; i < frameSprite.getHeight(); i++)
        {
        for(int j = 0; j < frameSprite.getWidth(); j++)
            {
            row.push_back(frameSprite.getPixelColor(j,i));
            }
        tempCurrent->paintRow(i,row);
        row.clear();
        }
    emit updateCurrent(*tempCurrent->getMap());
    MtimeIgnore = false;
}
