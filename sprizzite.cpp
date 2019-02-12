//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

#include "sprizzite.h"
#include "ui_sprizzite.h"
#include "QColorDialog"
#include "QPainter"
#include "QPixmap"
//test things:
#include <iostream>
#include "filedialog.h"
#include "QFileDialog"
#include "qfile.h"
#include "QTextStream"
#include "QString"
#include <string>
#include <QKeyEvent>
#include <QStatusBar>
#include "QCloseEvent"
#include "QShortcut"
#include "QRadioButton"
#include "qcheckbox.h"
#include "gif.h"


Sprizzite::Sprizzite(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Sprizzite)
{
    // Setup UI
    ui->setupUi(this);

    Mselecting = false;
    //Track mouse movement
    qApp->installEventFilter(this);

    MdrawMode = SELECT_DRAW_MODE; //Initially turn draw mode off
    McursorInCanvas = false;
    MsaveCancel = false;

    // File Menu options
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(OpenDialog()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveSprite()));
    QObject::connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(exportGif()));

    // Help Menu options
    QObject::connect(ui->actionHelpFile, SIGNAL(triggered(bool)), this, SLOT(openFileHelp()));
    QObject::connect(ui->actionHelpEdit, SIGNAL(triggered(bool)), this, SLOT(openEditHelp()));
    QObject::connect(ui->actionHelpView, SIGNAL(triggered(bool)), this, SLOT(openViewHelp()));
    QObject::connect(ui->actionHelpTools, SIGNAL(triggered(bool)), this, SLOT(openToolsHelp()));
    QObject::connect(ui->actionHelpColor, SIGNAL(triggered(bool)), this, SLOT(openColorPickerHelp()));
    QObject::connect(ui->actionHelpFrames, SIGNAL(triggered(bool)), this, SLOT(openFramesHelp()));
    QObject::connect(ui->actionHelpHotkeys, SIGNAL(triggered(bool)), this, SLOT(openHotkeysHelp()));

    // View Menu options
    QObject::connect(ui->actionDisplay_Grid, SIGNAL(triggered(bool)), &Mmodel, SLOT(toggleGrid(bool)));

    //Prompting and saving/opening
    QObject::connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(prompt()));
    QObject::connect(this, SIGNAL(promptSignal()), &newMenu, SLOT(opened()));
    QObject::connect(this, SIGNAL(changed()), &Mmodel, SLOT(changeFalse()));
    QObject::connect(&newMenu, SIGNAL(started()), &Mmodel, SLOT(projectStarted()));
    QObject::connect(this, SIGNAL(saved()), &Mmodel, SLOT(beenSaved()));
    QObject::connect(this, SIGNAL(startFromOpen()), &Mmodel, SLOT(projectStarted()));


    QObject::connect(&newMenu, SIGNAL(newFileSettings(int, int)), &Mmodel, SLOT(createNewFile(int,int)));
    QObject::connect(&Mmodel, SIGNAL(updateCanvas(QPixmap)), ui->Canvas, SLOT(setPixmap(const QPixmap &)));
    QObject::connect(this, SIGNAL(canvasResize(int, int)), &Mmodel, SLOT(updateCanvasSize(int, int)));

    // Sliders
    QObject::connect(ui->slider_r, SIGNAL(sliderMoved(int)), this, SLOT(on_slider_r_valueChanged(int)));
    QObject::connect(ui->slider_g, SIGNAL(sliderMoved(int)), this, SLOT(on_slider_g_valueChanged(int)));
    QObject::connect(ui->slider_b, SIGNAL(sliderMoved(int)), this, SLOT(on_slider_b_valueChanged(int)));

    // RGB text boxes
    QObject::connect(ui->RGBA_R, SIGNAL(textChanged(QString)), this, SLOT(on_RGBA_R_textEdited(const QString)));
    QObject::connect(ui->RGBA_G, SIGNAL(textChanged(QString)), this, SLOT(on_RGBA_G_textEdited(const QString)));
    QObject::connect(ui->RGBA_B, SIGNAL(textChanged(QString)), this, SLOT(on_RGBA_B_textEdited(const QString)));

    //These are for updating the frame caraousel
    QObject::connect(ui->nextButton, SIGNAL(pressed()), this, SLOT(nextFrame()));
    QObject::connect(ui->previousButton, SIGNAL(pressed()), this, SLOT(previousFrame()));
    QObject::connect(ui->deleteFrame, SIGNAL(pressed()), this, SLOT(deleteFrame()));
    QObject::connect(ui->addFrame, SIGNAL(pressed()), this, SLOT(newFrameWindow()));
    QObject::connect(this, SIGNAL(addFrame()), &Mmodel, SLOT(addFrameButton()));
    QObject::connect(this, SIGNAL(removeFrame()), &Mmodel, SLOT(frameDeleteButton()));
    QObject::connect(this, SIGNAL(frameNext()), &Mmodel, SLOT(nextFrame()));
    QObject::connect(this, SIGNAL(lastFrame()), &Mmodel, SLOT(previousFrame()));
    QObject::connect(&Mmodel, SIGNAL(updateCurrent(QPixmap)), ui->currentFrame, SLOT(setPixmap(QPixmap)));
    QObject::connect(&Mmodel, SIGNAL(updatePrevious(QPixmap)), ui->previousFrame, SLOT(setPixmap(QPixmap)));
    QObject::connect(&Mmodel, SIGNAL(updateNext(QPixmap)), ui->nextFrame, SLOT(setPixmap(QPixmap)));


    //This is for the preview box
    QObject::connect(&Mmodel, SIGNAL(timerFire(QPixmap)), ui->previewFrame, SLOT(setPixmap(const QPixmap &)));

    //Scroll Buttons
    QObject::connect(ui->rightScroll, SIGNAL(pressed()), &Mmodel, SLOT(scrollCanvasRight()));
    QObject::connect(ui->leftScroll, SIGNAL(pressed()), &Mmodel, SLOT(scrollCanvasLeft()));
    QObject::connect(ui->upScroll, SIGNAL(pressed()), &Mmodel, SLOT(scrollCanvasUp()));
    QObject::connect(ui->downScroll, SIGNAL(pressed()), &Mmodel, SLOT(scrollCanvasDown()));
    QObject::connect(&Mmodel, SIGNAL(refreshScrollButtonsSignal()), this, SLOT(refreshScrollButtons()));


    //Toolbar Frames/Preview actions
    QObject::connect(ui->actionAdd_Frame, SIGNAL(triggered()), this, SLOT(newFrameWindow()));
    QObject::connect(ui->actionDelete_Frame, SIGNAL(triggered()), this, SLOT(deleteFrame()));
    QObject::connect(ui->actionNext_Frame, SIGNAL(triggered()), this, SLOT(nextFrame()));
    QObject::connect(ui->actionPrevious_Frame, SIGNAL(triggered()), this, SLOT(previousFrame()));
    QObject::connect(ui->actionIncrease_Speed, SIGNAL(triggered()), &Mmodel, SLOT(speedUp()));
    QObject::connect(ui->actionDecrease_Speed, SIGNAL(triggered()), &Mmodel, SLOT(slowDown()));
    QObject::connect(ui->actionPause, SIGNAL(triggered()), &Mmodel, SLOT(pause()));
    QObject::connect(ui->actionPlay, SIGNAL(triggered()), &Mmodel, SLOT(play()));

    //Open File
    QObject::connect(this, SIGNAL(fileLoad(std::vector<Sprite>)), &Mmodel, SLOT(fileOpen(std::vector<Sprite>)));
    QObject::connect(&newMenu, SIGNAL(clearFrames()), this, SLOT(deleteFrames()));

    //Preview Speed connections
    QObject::connect(ui->slowerButton, SIGNAL(pressed()), &Mmodel, SLOT(slowDown()));
    QObject::connect(ui->fasterButton, SIGNAL(pressed()), &Mmodel, SLOT(speedUp()));
    QObject::connect(ui->pauseButton, SIGNAL(pressed()), &Mmodel, SLOT(pause()));
    QObject::connect(ui->playButton, SIGNAL(pressed()), &Mmodel, SLOT(play()));

    //copy/paste - just paste for now
    //QObject::connect(ui->pasteButton, SIGNAL(pressed()), &Mmodel, SLOT(pasteFromClipboard()));

    //Selection connections
    QObject::connect(this, SIGNAL(sendSelection(int,int,int,int)), &Mmodel, SLOT(selectionReleased(int,int,int,int)));
    QObject::connect(this, SIGNAL(deselect()), &Mmodel, SLOT(deselect()));
    QObject::connect(this, SIGNAL(setSelecting(bool)), &Mmodel, SLOT(setSelecting(bool)));
    QObject::connect(this, SIGNAL(drawSelecting(int,int,int,int)), &Mmodel, SLOT(drawSelecting(int,int,int,int)));

    //Zoom Connections
    //button names are backwards and I can't seem to fix them without losing icons
    QObject::connect(ui->zoomOut, SIGNAL(pressed()), &Mmodel, SLOT(zoomIn()));
    QObject::connect(ui->zoomIn, SIGNAL(pressed()), &Mmodel, SLOT(zoomOut()));
    QObject::connect(&Mmodel, SIGNAL(refreshZoomButtonsSignal(double)), this, SLOT(refreshZoomButtons(double)));

    //Fill Connection
    QObject::connect(ui->fillButton, SIGNAL(pressed()), &Mmodel, SLOT(fillSelection()));

    //Send colors to Mmodel
    QObject::connect(this, SIGNAL(sendPrimaryColor(QColor)), &Mmodel, SLOT(receivePrimaryColor(QColor)));
    QObject::connect(this, SIGNAL(sendSecondaryColor(QColor)), &Mmodel, SLOT(receiveSecondaryColor(QColor)));

    //Resets grid toggle value
    QObject::connect(&Mmodel, SIGNAL(sendGridSwitch(bool)), this, SLOT(checkGrid(bool)));

    //Rotate connection
    QObject::connect(this, SIGNAL(sendRotate(int, bool, bool)), &Mmodel, SLOT(rotateSelection(int, bool, bool)));

    // Set defaults
    ui->Color1->setStyleSheet("background: #000000;"); //Default pencil color is black
    ui->Color2->setStyleSheet("background: #ffffff;"); //Default canvas color is white

    //Set fonts to frame carasouel
    QFont font = ui->frameCurrent->font();
    font.setPointSize(11);
    font.setUnderline(true);
    ui->frameCurrent->setFont(font);


    /*
     * Shortcuts. No destructors required--QT takes care of deallocation on program end.
     */

    // Copy / Paste
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this, SLOT(on_copyButton_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_V), this, SLOT(on_pasteButton_clicked()));

    // Draw / Fill / Normal Pointer
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_D), this, SLOT(on_drawButton_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this, SLOT(on_fillButton_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this, SLOT(on_mousebutton_clicked()));

    // Zoom In / Zoom Out
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this, SLOT(on_zoomInButton_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus), this, SLOT(on_zoomOutButton_clicked()));

    // Erase / Rotate
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_X), this, SLOT(on_eraseButton_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this, SLOT(on_rotateButton_clicked()));

    // Open / Save / New
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this, SLOT(OpenDialog()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(saveSprite()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this, SLOT(prompt()));

    //Select the mouse icon as default
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    MdrawMode = SELECT_DRAW_MODE;
    ui->mousebutton->setFocus();
}

//destructor
Sprizzite::~Sprizzite()
{
    delete ui;
}

//Mouse Movement & Painting

bool Sprizzite::eventFilter(QObject *obj, QEvent *event)
{
    //initialize static select box locations
    static int selectLeft = 0;
    static int selectTop = 0;
    static int selectRight = 0;
    static int selectBot = 0;


    //only run draw events if cursor is in canvas
    if(obj == ui->Canvas) {
        // on mouse move
        if (event->type() == QEvent::MouseMove)
        {
            //set mouse location
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            int xPos = mouseEvent->pos().x();
            int yPos = mouseEvent->pos().y();
            statusBar()->showMessage(QString("(%1, %2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));

          //Selecting mouse move
          if(MdrawMode == SELECT_DRAW_MODE) {
              if(Mselecting) {
                  //set bottom right position to the new location
                  selectRight = xPos;
                  selectBot = yPos;

                  //send signal to redraw the selecting box
                  emit drawSelecting(selectLeft, selectTop, selectRight, selectBot);
              }
          }else if(MdrawMode == DRAW_DRAW_MODE) {
              //TODO: Impelement a check so it only allows drawing when a frame is available, otherwise crashes
              //^This can be implemented in the Mmodel with a simpl check.  See first line of selectionReleased method.

              //TODO:  make hard coded numbers constants in the header

              //Mmodel.drawtest(xPos, yPos, QColor(ui->RGBA_R->text().toInt(),ui->RGBA_G->text().toInt(),ui->RGBA_B->text().toInt(),ui->RGBA_A->text().toInt()));
              if(MdrawMoving == 1){
               Mmodel.draw(xPos, yPos, QColor(ui->RGBA_R->text().toInt(),ui->RGBA_G->text().toInt(),ui->RGBA_B->text().toInt()), ui->pensize_slider->value());
              }
            }else if (MdrawMode == ERASOR_DRAW_MODE) {
              //TODO: erasor code here
              if(MdrawMoving == 1){
              Mmodel.erase(xPos, yPos, ui->pensize_slider->value());
              }
          }
          //TODO: Constant draw and erase code here
        }

        //on mouse press
        else if(event->type() == QEvent::MouseButtonPress){
            //get x,y location
            int xPos = ui->Canvas->mapFromGlobal(QCursor::pos()).x();
            int yPos = ui->Canvas->mapFromGlobal(QCursor::pos()).y();

            if(MdrawMode == DRAW_DRAW_MODE) {
                MdrawMoving = 1; //Allow mouse draw while moving
                //TODO: Impelement a check so it only allows drawing when a frame is available, otherwise crashes
                //^This can be implemented in the Mmodel with a simpl check.  See first line of selectionReleased method.

                //TODO:  make hard coded numbers constants in the header

                //Mmodel.drawtest(xPos, yPos, QColor(ui->RGBA_R->text().toInt(),ui->RGBA_G->text().toInt(),ui->RGBA_B->text().toInt(),ui->RGBA_A->text().toInt()));
                Mmodel.draw(xPos, yPos, QColor(ui->RGBA_R->text().toInt(),ui->RGBA_G->text().toInt(),ui->RGBA_B->text().toInt()), ui->pensize_slider->value());
              }
            else if (MdrawMode == ERASOR_DRAW_MODE) {
                MdrawMoving = 1; //Allow mouse draw while moving
                //TODO: erasor code here
                Mmodel.erase(xPos, yPos, ui->pensize_slider->value());
            }
            else if (MdrawMode == SELECT_DRAW_MODE) {
                //set selection initial coords
                selectLeft = xPos;
                selectTop = yPos;

                //send signal to deselect previous selection
                emit(deselect());

                //turn on selecting
                Mselecting = true;

                //send signal to set the new select location
                emit(setSelecting(Mselecting));
            }

        }

        //On mouse release event
        else if(event->type() == QEvent::MouseButtonRelease){
            //get location
            int xPos = ui->Canvas->mapFromGlobal(QCursor::pos()).x();
            int yPos = ui->Canvas->mapFromGlobal(QCursor::pos()).y();

            MdrawMoving = 0; //Allow mouse draw while moving

            //on selection release
            if(MdrawMode == SELECT_DRAW_MODE) {
                //set new selection coords
                selectRight = xPos;
                selectBot = yPos;

                //turn off selecting
                Mselecting = false;

                //send new selection to model
                emit(sendSelection(selectLeft, selectTop, selectRight, selectBot));

                //send signal to turn off selecting in model
                emit(setSelecting(Mselecting));
            }

        }
    }

    //Keeps track of if the cursor is over the canvas
    if(event->type() == QEvent::Enter && obj == ui->Canvas) {
        McursorInCanvas = true;
    }

    //keeps track of if the cursor is over the canvas
    else if(event->type() == QEvent::Leave && obj == ui->Canvas) {
        McursorInCanvas = false;
    }
    return false;
}

    // Resize Event& refresh Scroll Buttons
void Sprizzite::resizeEvent(QResizeEvent *event){
    emit canvasResize(ui->Canvas->size().width(), ui->Canvas->size().height());
    refreshScrollButtons();
}

void Sprizzite::prompt()
{
    if(Mmodel.MStartedProject && Mmodel.Mchanged)
        {
        QMessageBox msgBox(QMessageBox::Question,"Sprizzite", "<p align='center'>Do you want to save before starting new project?</p>",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this,Qt::FramelessWindowHint);
        switch(msgBox.exec())
            {
            case QMessageBox::Yes:
                {
                saveSprite();
                emit promptSignal();
                }
            case QMessageBox::No:
                {
                emit promptSignal();
                }
            case QMessageBox::Cancel:
                {
                return;
                }
            default:
                {
                return;
                }
            }
        }
    else
        {
        emit promptSignal();
        }
}

void Sprizzite::closeEvent (QCloseEvent *event)
{
    if(Mmodel.Mchanged)
        {
        QMessageBox msgBox(QMessageBox::Question,"Sprizzite", "<p align='center'>Do you want to save before quitting?</p>",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this,Qt::FramelessWindowHint);
        switch(msgBox.exec())
            {
            case QMessageBox::Yes:
                {
                saveSprite();
                if(MsaveCancel == true)
                    {
                    event->ignore();
                    }
                return;
                }
            case QMessageBox::No:
                {
                return;
                }
            case QMessageBox::Cancel:
                {
                    event->ignore();
                }
            default:
                {
                event->ignore();
                }
            }
        }
    else
        {
        event->accept();
        }
}

//Updates view to show frame that was just created.
void Sprizzite::newFrameWindow()
{
    if(Mmodel.MStartedProject)
        {
        ui->frameCurrent->setText("Frame " + QString::number(Mmodel.Msprites.size()+1));
        emit addFrame();
        refreshScrollButtons();
        }
}
//Updates view to next frame to show which frame we are on.
void Sprizzite::nextFrame()
{
    if(Mmodel.MStartedProject)
        {
         if(ui->frameCurrent->text().remove(0,5).toInt() == Mmodel.Msprites.size())
            {
            return;
            }
            else
            {
            ui->frameCurrent->setText("Frame " + QString::number(ui->frameCurrent->text().remove(0,5).toInt()+1));
            emit frameNext();
            }
        }
}


//Updates view to previous frame to show which frame we are on
void Sprizzite::previousFrame()
{
    if(Mmodel.MStartedProject)
        {
        if(ui->frameCurrent->text().remove(0,5).toInt() == 1)
            {
            return;
            }
        else
            {
            ui->frameCurrent->setText("Frame " + QString::number(ui->frameCurrent->text().remove(0,5).toInt()-1));
            emit lastFrame();
            }
        }
}

//Delete Frame
void Sprizzite::deleteFrame()
{
    if(Mmodel.MStartedProject)
        {
        if(ui->frameCurrent->text().remove(0,5).toInt() == 1)
            {
            emit removeFrame();
            }
        else
            {
            ui->frameCurrent->setText("Frame " + QString::number(ui->frameCurrent->text().remove(0,5).toInt()-1));
            emit removeFrame();
            }
        }
}

void Sprizzite::deleteFrames()
{
    ui->frameCurrent->setText("Frame 1");
}
    //Clickable Color 1
void Sprizzite::on_Color1_clicked()
{
    QColor color = QColorDialog::getColor();
    emit sendPrimaryColor(color);

    // Update text box with selected color values
    ui->RGBA_R->setText(QString::number((color.red())));
    ui->RGBA_G->setText(QString::number((color.green())));
    ui->RGBA_B->setText(QString::number((color.blue())));

    // Update slider with selected color values
    ui->slider_r->setSliderPosition(color.red());
    ui->slider_g->setSliderPosition(color.green());
    ui->slider_b->setSliderPosition(color.blue());

    QString s(
        "background: #" +
        QString(color.red() < 16? "0" : "") + QString::number(color.red(),16) +
        QString(color.green() < 16? "0" : "") + QString::number(color.green(),16) +
        QString(color.blue() < 16? "0" : "") + QString::number(color.blue(),16) + ";"
    );
    ui->Color1->setStyleSheet(s);
    ui->Color1->update();

    //TODO: get the proper canvas from the Mmodel and set the color
}
    //clickable Color 2
void Sprizzite::on_Color2_clicked()
{
    QColor color = QColorDialog::getColor();
    emit sendSecondaryColor(color);

    QString s(
        "background: #" +
        QString(color.red() < 16? "0" : "") + QString::number(color.red(),16) +
        QString(color.green() < 16? "0" : "") + QString::number(color.green(),16) +
        QString(color.blue() < 16? "0" : "") + QString::number(color.blue(),16) + ";"
    );
    ui->Color2->setStyleSheet(s);
    ui->Color2->update();

    //TODO: get the proper canvas from the Mmodel and set the color
    //scene->color = color;
   //sprite.setForegroundColor(color);
}

/*
 * The next three methods are slots used to save, open, or export a Sprizzite project from the file menu.
 */

// The exportGif method opens a file dialog to select a file location and name and then
// creates an animated gif file from the frames of a Sprizzite project at the chosen directory.
void Sprizzite::exportGif() {
    if(Mmodel.MStartedProject == false)
    {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export"),"/home/",tr("Image (*.gif)"));

    if (fileName.isEmpty()) {
        return;
    } else {
        // get relevant data from Mmodel, write to file.
        int numFrames = Mmodel.Msprites.size();
        int height = 0;
        int width = 0;
        if (numFrames != 0) {
             height = Mmodel.Msprites[0].getHeight();
             width = Mmodel.Msprites[0].getWidth();
        }
        // Frames per second
        int fps = 15;

        //Start writing to a gif file
        GifWriter writer;
        GifBegin(&writer, fileName.toStdString().c_str(), width, height, 100 / fps, 8, false);
        // Store values of a frame to be written to a gif in an array of unsigned characters
        uint8_t * image = new uint8_t[4 * width * height];
        // Go through each frame
        for (auto&& s : Mmodel.Msprites){
            // Retrieve the color of each pixel
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    QColor colorHere = s.getPixelColor(x,y);
                    // Retrieve the individual rgb values of each pixel ("a" values are ignored by gif.h)
                    image[((y * width + x) * 4)] = colorHere.red();
                    image[((y * width + x) * 4) + 1] = colorHere.green();
                    image[((y * width + x) * 4) + 2] = colorHere.blue();
                }
            }
            // Write frame to the gif file
            GifWriteFrame(&writer, image, width, height, 100 / fps, 8, false);
         }
         // Close the gif file
         GifEnd(&writer);
    }
}

// for Action - opens a file dialog to select a save file location, then writes the Mmodel data
// into said file for later use.
void Sprizzite::saveSprite()
{
    if(Mmodel.MStartedProject == false)
        {
        return;
        }
     QString fileName;
    if(Mmodel.MhasBeenSaved == true)
        {
        fileName = MsavedFileName;
        }
    else
        {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As"),"/home/",tr("Sprite Files (*.ssp)"));
        MsavedFileName = fileName;
        }

    if (fileName.isEmpty()) {
        MsaveCancel = true;
        return;
    } else

    {
       // get relevant data from Mmodel, write to file.
       int numFrames = Mmodel.Msprites.size();
       int height = 0;
       int width = 0;

       if (numFrames != 0) {
            height = Mmodel.Msprites[0].getHeight();
            width = Mmodel.Msprites[0].getWidth();
       }
       QFile newSpriteFile(fileName);
       if (!newSpriteFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
           return;

       }
       QTextStream out(&newSpriteFile);

       out << height << " " << width << '\n';
       out << numFrames << '\n';

       int countSprites = 0; //we don't want a trailing newline.

       //write frames, pixel by pixel
       for (auto&& s : Mmodel.Msprites){
           for (int x = 0; x < height; x++) {
               for (int y = 0; y < width; y++) {
                   QColor colorHere = s.getPixelColor(x,y);
                   int r =0;
                   int g = 0;
                   int b = 0;
                   int a = 0;
                   colorHere.getRgb(&r, &g, &b, &a);
                   out << r << " " << g << " " << b << " " << a;
                   //if we are not at the end, space separate
                   if (y != width-1) {
                       out << " ";
                   }
               }
               if(x!= height-1 || countSprites!=numFrames-1) {
                   out << '\n';
               }
           }
           countSprites++;
        }
        newSpriteFile.close();
    }



    emit changed();
    emit saved();

}

void Sprizzite::OpenDialog() {

  if(Mmodel.Mchanged)
    {
    QMessageBox msgBox(QMessageBox::Question,"Sprizzite", "<p align='center'>Do you want to save before opening a new project?</p>",
    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this,Qt::FramelessWindowHint);
    switch(msgBox.exec())
        {
        case QMessageBox::Yes:
            {
            saveSprite();
            break;
            }
        case QMessageBox::No:
            {
            break;
            }
        case QMessageBox::Cancel:
            {
            return;
            }
        default:
            {
            return;
            }
        }
    }

        // Open file Name
    MsavedFileName = QFileDialog::getOpenFileName(this,
        tr("Open Sprite"), "/home/", tr("Sprite Files (*.ssp)"));
    //std::cout << "file grabbed: " << fileName.toStdString() <<std::endl;

    QFile spriteFile(MsavedFileName);
    if (!spriteFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout<< "failure?" << std::endl;
        return;
    }

    QTextStream in(&spriteFile);

    if (in.atEnd()) {
        return; //this is a fail case, file was corrupt
    }
    //file contract: 2 integers for H and W, newline, then another integer
    //then frames - each line is a row of pixel values. space separated, 4 values per pix
    QString line = in.readLine(); //stoi = string to int. TODO: catch error here

    int H = std::stoi(line.toStdString().substr(0, line.toStdString().find_first_of(' ')));
    int W = std::stoi(line.toStdString().substr(line.toStdString().find_first_of(' ')+1));

    line = in.readLine();
    int numFrames = std::stoi(line.toStdString());

    std::vector<Sprite> Frames; //should be filled in while loop above

    int row = 0;
    Sprite nextFrame{H,W};

    while (!in.atEnd()) {
        line = in.readLine();
        if (row == 0) {
            nextFrame = Sprite{H, W};
        }
        //create an array to store the color values.
        QStringList colorValues = line.split(" ");
        for (int i = 0; i < 4*W; i+= 4) {
            QColor color(std::stoi(colorValues[i].toStdString()), std::stoi(colorValues[i+1].toStdString()),
                std::stoi(colorValues[i+2].toStdString()), std::stoi(colorValues[i+3].toStdString()));
            nextFrame.changePixelColor(row, i>>2, color);
        }
        row++;
        if (row == H) {
            row = 0;
            Frames.push_back(nextFrame);
        }
    }

    QString Name = QString::fromStdString(MsavedFileName.toStdString().substr(MsavedFileName.lastIndexOf('/') +1));

    spriteFile.close();

    emit startFromOpen();

    emit saved();

    emit fileLoad(Frames);

    deleteFrames();

    ui->frameCurrent->setText("Frame " + QString::number(Mmodel.Msprites.size()));
}

/* Message alert to show that the project needs to be started before using a tool or function */
void Sprizzite::ProjectNotStartedAlert(){
    //QMessageBox msgBox;
    //msgBox.setInformativeText("To use this tool, you must first start a new project.\n To start a new project, use the CTRL + N shortcut or use the File menu.");
    //msgBox.exec();
    ui->mousebutton->setFocus();
}

/*
 * This group of methods changes the cursor icon depending
 * on which button is clicked.
 */
void Sprizzite::on_drawButton_clicked()
{
    if(Mmodel.MStartedProject == true){
        QApplication::setOverrideCursor(QCursor(QPixmap(":/buttons/buttons/pencilcursor.png")));
        //Switch to paint mode
        //Make a trigger here...
        MdrawMode = DRAW_DRAW_MODE;

        //sends signal to deselect in model
        emit(deselect());
    }else{
        this->ProjectNotStartedAlert();
        return;
    }
}
    // Activate Erase Button
void Sprizzite::on_eraseButton_clicked()
{
    if(Mmodel.MStartedProject == true){
        QApplication::setOverrideCursor(QCursor(QPixmap(":/buttons/buttons/erasercursor.png")));
        MdrawMode = ERASOR_DRAW_MODE;

        //sends signal to deselect in model
        emit deselect();

    }else{
        this->ProjectNotStartedAlert();
        return;
    }
}
    //clickable Mouse Button
void Sprizzite::on_mousebutton_clicked()
{

        QApplication::setOverrideCursor(Qt::ArrowCursor);
        MdrawMode = SELECT_DRAW_MODE;

        //sends signal to deselect in model
        emit deselect();
}
    // Function for Fill Button
void Sprizzite::on_fillButton_clicked()
{
    if(Mmodel.MStartedProject == true){
        }else{
            this->ProjectNotStartedAlert();
            return;
        }

}

    //Function for Rotate Button
void Sprizzite::on_rotateButton_clicked()
{
    //creates message asking for rotation degrees
    if (!Mmodel.MStartedProject) {this->ProjectNotStartedAlert();return;}
    QMessageBox msgBox(QMessageBox::Question,"Sprizzite", "<p align='center'>Select Degrees Clockwise:</p>", QMessageBox::Cancel, this,Qt::FramelessWindowHint);

    //create radio buttons
    QRadioButton zero(tr("0 Degrees"), this);
    QRadioButton ninety(tr("90 Degrees"), this);
    QRadioButton oneEighty(tr("180 Degrees"), this);
    QRadioButton twoSeventy(tr("270 Degrees"), this);

    //add buttons to message
    msgBox.addButton(&zero, QMessageBox::ActionRole);
    msgBox.addButton(&ninety, QMessageBox::ActionRole);
    msgBox.addButton(&oneEighty, QMessageBox::ActionRole);
    msgBox.addButton(&twoSeventy, QMessageBox::ActionRole);

    //set degrees to input
    int rotationAmount = 0;
    msgBox.exec();
    if (msgBox.clickedButton() == &ninety) {
        rotationAmount = 90;
    } else if (msgBox.clickedButton() == &oneEighty) {
        rotationAmount = 180;
    } else if (msgBox.clickedButton() == &twoSeventy) {
        rotationAmount = 270;
    } else if (msgBox.clickedButton() != &zero) {
        return;
    }

    //creates message asking for mirror
    QMessageBox msgBox2(QMessageBox::Question,"Sprizzite", "<p align='center'>Mirrored?</p>", QMessageBox::Cancel, this,Qt::FramelessWindowHint);

    //creates buttons
    QRadioButton horiz(tr("Horizontal"), this);
    QRadioButton vert(tr("Vertical"), this);
    QRadioButton none(tr("None"), this);

    //adds buttons to message
    msgBox2.addButton(&horiz, QMessageBox::ActionRole);
    msgBox2.addButton(&vert, QMessageBox::ActionRole);
    msgBox2.addButton(&none, QMessageBox::ActionRole);

    //sets result to input
    bool vertical = false;
    bool horizontal = false;
    msgBox2.exec();
    if(msgBox2.clickedButton() == &horiz) horizontal = true;
    else if (msgBox2.clickedButton() == &vert) vertical = true;

    //send signal to rotate and deselect
    emit sendRotate(rotationAmount, vertical, horizontal);
}
    // Funtion for Paste Button
void Sprizzite::on_pasteButton_clicked()
{

    if (!Mmodel.MStartedProject) {this->ProjectNotStartedAlert();return;}

    Mmodel.pasteFromClipboard();
    emit deselect();
}
    //Function for Copy Button
void Sprizzite::on_copyButton_clicked()
{

    if (!Mmodel.MStartedProject) {this->ProjectNotStartedAlert();return;}
    Mmodel.copyToClipboard();

}
    //Function for ZoomOut
void Sprizzite::on_zoomOutButton_clicked()
{
    if(Mmodel.MStartedProject == true){
        }else{
            this->ProjectNotStartedAlert();
            return;
        }
}
void Sprizzite::on_zoomInButton_clicked()
{
    if(Mmodel.MStartedProject == true){
        }else{
            this->ProjectNotStartedAlert();
            return;
        }

}


/*
 * These next three methods update the RGB input boxes and the color picker
 * with the slider value whenever it is moved.
 *
 * Text boxes and color picker will update as you slide.
 *
 * Values range from 0 - 255.
*/
void Sprizzite::on_slider_r_valueChanged(int position)
{
    // Set text box value to that of the slider
    ui->RGBA_R->setText(QString::number(position));

    // Change the color pickers color to that of the changed value
    QString s(
        "background: #" +
        QString(position < 16? "0" : "") + QString::number(position,16) +
        QString(ui->RGBA_G->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_G->text().toInt(),16) +
        QString(ui->RGBA_B->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_B->text().toInt(),16) + ";"
    );
    ui->Color1->setStyleSheet(s);
    ui->Color1->update();

    emit sendPrimaryColor( QColor(ui->RGBA_R->text().toInt(),ui->RGBA_G->text().toInt(),ui->RGBA_B->text().toInt()) );
}
void Sprizzite::on_slider_g_valueChanged(int position)
{
    // Set text box value to that of the slider
    ui->RGBA_G->setText(QString::number(position));

    // Change the color pickers color to that of the changed value
    QString s(
        "background: #" +
        QString(ui->RGBA_R->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_R->text().toInt(),16) +
        QString(position < 16? "0" : "") + QString::number(position,16) +
        QString(ui->RGBA_B->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_B->text().toInt(),16) + ";"
    );
    ui->Color1->setStyleSheet(s);
    ui->Color1->update();

    emit sendPrimaryColor( QColor(ui->RGBA_R->text().toInt(),ui->RGBA_G->text().toInt(),ui->RGBA_B->text().toInt()) );

}
void Sprizzite::on_slider_b_valueChanged(int position)
{
    // Set text box value to that of the slider
    ui->RGBA_B->setText(QString::number(position));

    // Change the color pickers color to that of the changed value
    QString s(
        "background: #" +
        QString(ui->RGBA_R->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_R->text().toInt(),16) +
        QString(ui->RGBA_G->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_G->text().toInt(),16) +
        QString(position < 16? "0" : "") + QString::number(position,16) + ";"
    );
    ui->Color1->setStyleSheet(s);
    ui->Color1->update();

    emit sendPrimaryColor( QColor(ui->RGBA_R->text().toInt(),ui->RGBA_G->text().toInt(),ui->RGBA_B->text().toInt()) );

}

/*
 * These next three methods update the RGB sliders and the color picker
 * with the values in the input boxes whenever it is changed.
 *
 * Slider and color picker will update as you type.
 *
 * If input is not an integer between 0 and 255, the slider will default to
 * the 0 position.
 *
 * This also inhibits typing anything other than integer values between 0 and 255.
*/
void Sprizzite::on_RGBA_R_textEdited(const QString &arg1)
{
    //  the slider
    int value = arg1.toInt();
    if (value > 255 || value < 0)
        value = 0;
    ui->slider_r->setSliderPosition(value);
    // This line will set the textbox text to 0 if the input is anything
    // that is not an integer from 0-255
    ui->RGBA_R->setText(QString::number(value));

    // Update the color picker
    QString s(
        "background: #" +
        QString(value < 16? "0" : "") + QString::number(value,16) +
        QString(ui->RGBA_G->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_G->text().toInt(),16) +
        QString(ui->RGBA_B->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_B->text().toInt(),16) + ";"
    );
    ui->Color1->setStyleSheet(s);
    ui->Color1->update();

    emit sendPrimaryColor( QColor(ui->RGBA_R->text().toInt(),ui->RGBA_G->text().toInt(),ui->RGBA_B->text().toInt()) );

}
void Sprizzite::on_RGBA_G_textEdited(const QString &arg1)
{
    // Update the slider
    int value = arg1.toInt();
    if (value > 255 || value < 0)
        value = 0;
    ui->slider_g->setSliderPosition(value);
    // This line will set the textbox text to 0 if the input is anything
    // that is not an integer from 0-255
    ui->RGBA_G->setText(QString::number(value));

    // Update the color picker
    QString s(
        "background: #" +
        QString(ui->RGBA_R->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_R->text().toInt(),16) +
        QString(value < 16? "0" : "") + QString::number(value,16) +
        QString(ui->RGBA_B->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_B->text().toInt(),16) + ";"
    );
    ui->Color1->setStyleSheet(s);
    ui->Color1->update();

    emit sendPrimaryColor( QColor(ui->RGBA_R->text().toInt(),ui->RGBA_G->text().toInt(),ui->RGBA_B->text().toInt()) );

}
void Sprizzite::on_RGBA_B_textEdited(const QString &arg1)
{
    // Update the slider
    int value = arg1.toInt();
    if (value > 255 || value < 0)
        value = 0;
    ui->slider_b->setSliderPosition(value);
    // This line will set the textbox text to 0 if the input is anything
    // that is not an integer from 0-255
    ui->RGBA_B->setText(QString::number(value));

    // Update the color picker
    QString s(
        "background: #" +
        QString(ui->RGBA_R->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_R->text().toInt(),16) +
        QString(ui->RGBA_G->text().toInt() < 16? "0" : "") + QString::number(ui->RGBA_G->text().toInt(),16) +
        QString(value < 16? "0" : "") + QString::number(value,16) + ";"
    );
    ui->Color1->setStyleSheet(s);
    ui->Color1->update();

    emit sendPrimaryColor( QColor(ui->RGBA_R->text().toInt(),ui->RGBA_G->text().toInt(),ui->RGBA_B->text().toInt()) );

}
    //Refresh the Scroll Buttons

//enables scroll buttons if the canvas can move that direction
//disables otherwise
void Sprizzite::refreshScrollButtons() {
    if(Mmodel.Mcanvas == nullptr) return;
    if(Mmodel.Mvp->getLeft() == 0) ui->leftScroll->setEnabled(false);
    else ui->leftScroll->setEnabled(true);
    if(Mmodel.Mvp->getTop() == 0) ui->upScroll->setEnabled(false);
    else ui->upScroll->setEnabled(true);
    if(Mmodel.Mvp->getRight() >= Mmodel.getActiveSprite().getWidth()) ui->rightScroll->setEnabled(false);
    else ui->rightScroll->setEnabled(true);
    if(Mmodel.Mvp->getBot() >= Mmodel.getActiveSprite().getHeight()) ui->downScroll->setEnabled(false);
    else ui->downScroll->setEnabled(true);
}
    //Open File Help
void Sprizzite::openFileHelp(){
    QMessageBox help;
    help.setWindowTitle("File Help");
    help.setText("File: \n \n"
                 "      New - Creates a new project. \n\n"
                 "      Open - Opens an existing project. \n\n"
                 "      Save - Save the currently open project. \n\n"
                 "      Export - Export the current project as GIF. \n");
    help.setWindowFlags(Qt::CustomizeWindowHint);
    help.exec();
}
    //Function for edit help
void Sprizzite::openEditHelp(){
    QMessageBox help;
    help.setWindowTitle("Edit Help");
    help.setText("Edit: \n \n"
                 "      Frames - Add, Remove, and View the Next/Previous frames. \n\n"
                 "      Preview - Play, Pause, and Speedup/Slowdown frame animation. \n");
    help.setWindowFlags(Qt::CustomizeWindowHint);
    help.exec();
}
    //Open view Hlep
void Sprizzite::openViewHelp(){
    QMessageBox help;
    help.setWindowTitle("View Help");
    help.setText("View: \n \n"
                 "      Grid - Toggle displaying the pixel grid on the canvas. \n");
    help.setWindowFlags(Qt::CustomizeWindowHint);
    help.exec();
}
    //Open Tools Help
void Sprizzite::openToolsHelp(){
    QMessageBox help;
    help.setWindowTitle("Tools Help");
    help.setText("Tools: \n \n"
                 "      Cursor - Click to use your normal cursor. \n\n"
                 "      Pencil - Click to draw on the frame.\n\n"
                 "      Eraser - Click to remove from the frame. \n\n"
                 "      Zoom In - Click to zoom in on the frame. \n\n"
                 "      Zoom Out - Click to zoom out from the frame. \n\n"
                 "      Copy - Click to copy a selection. \n\n"
                 "      Paste - Click to paste at selection location. \n\n"
                 "      Rotate - Click to open dialogs for rotate. \n\n"
                 "      Fill - Click to fill the selection. \n");
    help.setWindowFlags(Qt::CustomizeWindowHint);
    help.exec();
}
    // open Color Picker Message Box
void Sprizzite::openColorPickerHelp(){
    QMessageBox help;
    help.setWindowTitle("Color Picker Help");
    help.setText("Color Picker: \n \n"
                 "      Sliders - Slide to change the color values (RGB). \n\n"
                 "      Text Boxes - Manually input RGB color values. \n\n"
                 "      Color Boxes - Click for a user-friendly color picker. \n\n"
                 "      Pen Size - Drop down menu to select drawing pen size. \n");
    help.setWindowFlags(Qt::CustomizeWindowHint);
    help.exec();
}
    // Open Frames Help
void Sprizzite::openFramesHelp(){
    QMessageBox help;
    help.setWindowTitle("Frames Help");
    help.setText("Frames: \n \n"
                 "      Previous - Show previous frame. \n\n"
                 "      Next - Show next frame. \n\n"
                 "      Delete Frame (-) - Delete current frame. \n\n"
                 "      Add Frame (+) - Add another frame. \n\n"
                 "      Backward arrows - Slow down animation. \n\n"
                 "      Forward arrows - Speedup animation. \n\n"
                 "      Play - Plays frame animation. \n\n"
                 "      Pause - Pauses animation. \n");
    help.setWindowFlags(Qt::CustomizeWindowHint);
    help.exec();
}
    // Function for Hot Keys
void Sprizzite::openHotkeysHelp(){
    QMessageBox help;
    help.setWindowTitle("Hotkeys List");
    help.setText("Hotkeys: \n \n"
                 "      Ctrl + O - Open a project. \n"
                 "      Ctrl + S - Save current project. \n"
                 "      Ctrl + N - Start new project. \n\n"
                 "      Ctrl + C - Copy selection. \n"
                 "      Ctrl + V - Paste a selection. \n\n"
                 "      Ctrl + D - Select Draw button. \n"
                 "      Ctrl + F - Select Fill button. \n"
                 "      Ctrl + A - Select Cursor button. \n\n"
                 "      Ctrl + X - Delete a selection. \n"
                 "      Ctrl + R - Rotate a selection. \n\n"
                 "      Ctrl + '+' - Zoom in on frame. \n"
                 "      Ctrl + '-' - Zoom out from frame. \n");
    help.setWindowFlags(Qt::CustomizeWindowHint);
    help.exec();
}

//enables or disables the zoom buttons
void Sprizzite::refreshZoomButtons(double currentZoom) {
    if (currentZoom == MINIMUM_ZOOM) ui->zoomIn->setEnabled(false);
    else ui->zoomIn->setEnabled(true);
    if (currentZoom == MAXIMUM_ZOOM) ui->zoomOut->setEnabled(false);
    else ui->zoomOut->setEnabled(true);
}

//checks the toggle grid option on or off
void Sprizzite::checkGrid(bool on) {
    ui->actionDisplay_Grid->setChecked(on);
}
