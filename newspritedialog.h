#ifndef NEWSPRITEDIALOG_H
#define NEWSPRITEDIALOG_H

#include <QDialog>

namespace Ui {
class NewSpriteDialog;
}

class NewSpriteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewSpriteDialog(QWidget *parent = 0);
    ~NewSpriteDialog();

public slots:
    void display();

private:
    Ui::NewSpriteDialog *ui;
};

#endif // NEWSPRITEDIALOG_H
