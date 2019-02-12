//SEGFAULT
//Nathan Donaldson
//Meysam Hamel
//Scott Neville
//Aaron Mcghie
//Eric Miramontes
//Cashe Rumsey

//Header file for fileDialog; signals and slots defined here

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QWidget>
#include "QMessageBox"
#include "model.h"

namespace Ui {
class FileDialog;
}

class FileDialog : public QWidget
{
    Q_OBJECT

public:
    explicit FileDialog(QWidget *parent = 0);
    ~FileDialog();

private:
    Ui::FileDialog *ui;

signals:
    void newFileSettings(int W, int H);

    //used for prompting
    void saved();
    void started();
    void clearFrames();
    void newFrameWindow();

public slots:
    void opened();
    void FileCancel();
    void FileNew();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

};

#endif // FILEDIALOG_H
