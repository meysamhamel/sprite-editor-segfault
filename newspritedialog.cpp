#include "newspritedialog.h"
#include "ui_newspritedialog.h"
#include <iostream>

NewSpriteDialog::NewSpriteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSpriteDialog)
{
    ui->setupUi(this);
}

NewSpriteDialog::~NewSpriteDialog()
{
    delete ui;
}

void NewSpriteDialog::display() {
    std::cout << "HI!";
}
