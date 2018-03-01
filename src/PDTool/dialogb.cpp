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

    seconds=60;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);


    receivetask(task_ad);
}

Dialogb::~Dialogb()
{
    delete ui;
}


void Dialogb::on_pushButton_close_image_clicked()
{
   this->close();
}

void Dialogb::receivetask(int task)
{
    seconds=60;
    QString task_picture="";
    if (task==0){
        task_picture=task_image0;
    }
    if (task==16){
        task_picture=task_image16;
    }

    ui->label_image->setPixmap(QPixmap( task_picture ) );
}


void Dialogb::showTime()
{
    seconds=seconds-1;
    QString s = QString::number(seconds);
    ui->label_time->setText(s);
    if (seconds==0){this->close();}
}
