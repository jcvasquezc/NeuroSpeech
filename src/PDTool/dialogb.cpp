#include "dialogb.h"
#include "ui_dialogb.h"
#include <QMainWindow>
#include <QDialog>
#include "strings.h"


Dialogb::Dialogb(QWidget *parent, int task_ad) :
    QDialog(parent),
    ui(new Ui::Dialogb)
{
    ui->setupUi(this);
    receivetask(task_ad);


}

Dialogb::~Dialogb()
{
    delete ui;
}


void Dialogb::on_pushButton_close_image_clicked()
{
   int a=1;
}

void Dialogb::receivetask(int task)
{
    QString task_picture="";
    if (task==0){
        task_picture=task_image0;
    }
    if (task==16){
        task_picture=task_image16;
    }

    ui->label_image->setPixmap(QPixmap( task_picture ) );
}
