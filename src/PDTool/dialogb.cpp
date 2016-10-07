#include "dialogb.h"
#include "ui_dialogb.h"
#include <QMainWindow>
#include <QDialog>


Dialogb::Dialogb(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialogb)
{
    ui->setupUi(this);
    QString task1="/A/";
    QString task2="/I/";
    QStringList list;
    list <<task1;
    list.append(task2);
    ui->listWidget->addItems(list);


}

Dialogb::~Dialogb()
{
    delete ui;
}


void Dialogb::on_listWidget_clicked(const QModelIndex &index)
{
    int a=1;
}
