//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

//Header file for sprizzite class, declare methods, slots, signals, and member variables.

#ifndef SPRIZZITE_H
#define SPRIZZITE_H

#include <QMainWindow>
#include <QColorDialog>
#include <QPainter>
#include <QPixmap>
#include <QMessageBox>
#include <QDebug>
#include "filedialog.h"

namespace Ui {
class Sprizzite;
}

class Sprizzite : public QMainWindow
{
    Q_OBJECT
    FileDialog newMenu;
    QString savedFileName;

    //Constants
    //Draw Mode values
    const int SELECT_DRAW_MODE = 0;
    const int DRAW_DRAW_MODE = 1;
    const int ERASOR_DRAW_MODE = 2;
    const int PASTE_MODE = 3;
    //Min and max zoom multipliers
    const double MINIMUM_ZOOM = 1.0;
    const double MAXIMUM_ZOOM = 32.0;

    //members
    int MdrawMode;
    int MdrawMoving = 0;
    bool McursorInCanvas;
    bool Mselecting;
    bool MsaveCancel;


public:
    explicit Sprizzite(QWidget *parent = 0);
    Model Mmodel;
    QString MsavedFileName;
    ~Sprizzite();

signals:
    void fileLoad(std::vector<Sprite> Frames);

    void canvasResize(int w, int h);
    //selection signals
    void setSelecting(bool);
    void sendSelection(int, int, int, int);
    void deselect();
    void drawSelecting(int, int, int, int);
    //scroll button signals
    void scrollRightPushed();
    void scrollLeftPushed();
    void scrollUpPushed();
    void scrollDownPushed();
    //used for prompts
    void promptSignal();
    void changed();
    void saved();
    void startFromOpen();
    //Used for caraousel changes
    void addFrame();
    void removeFrame();
    void frameNext();
    void lastFrame();
    //Color picking signals
    void sendPrimaryColor(QColor);
    void sendSecondaryColor(QColor);
    //Rotate signal int is number of degrees, bool if mirrored, bool if mirrored horiz
    void sendRotate(int, bool, bool);

public slots:
    void OpenDialog();
    void saveSprite();
    void exportGif();
    void closeEvent(QCloseEvent*);
    //activates/deactivates scroll buttons as necessary
    void refreshScrollButtons();
    //activates/deactivates zoom buttons as necessary.  Takes in current zoom multiplier
    void refreshZoomButtons(double);
    //set display grid checked
    void checkGrid(bool);

private slots:
    void on_Color1_clicked();
    void on_Color2_clicked();
    //File/new, next/previous button methods.
    void newFrameWindow();
    void nextFrame();
    void previousFrame();
    void deleteFrames();
    void deleteFrame();
    //Prompt user to save if unsaved changes detected
    void prompt();
    // Help Menu
    void openFileHelp();
    void openEditHelp();
    void openViewHelp();
    void openToolsHelp();
    void openColorPickerHelp();
    void openFramesHelp();
    void openHotkeysHelp();

    void on_drawButton_clicked();
    void on_eraseButton_clicked();
    void on_mousebutton_clicked();
    void on_fillButton_clicked();
    void on_rotateButton_clicked();
    void on_pasteButton_clicked();
    void on_copyButton_clicked();
    void on_zoomOutButton_clicked();
    void on_zoomInButton_clicked();
    // When changing rgb values with the textboxes
    void on_RGBA_R_textEdited(const QString &arg1);
    void on_RGBA_G_textEdited(const QString &arg1);
    void on_RGBA_B_textEdited(const QString &arg1);
    // When changing rgb values with the sliders
    void on_slider_r_valueChanged(int value);
    void on_slider_g_valueChanged(int value);
    void on_slider_b_valueChanged(int value);


    //void on_zoomOut_clicked();

private:
    Ui::Sprizzite *ui;
    void ProjectNotStartedAlert();

protected:
    //Event triggered to emit signal when window is resized
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // SPRIZZITE_H
