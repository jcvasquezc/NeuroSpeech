#ifndef DIALOGB_H
#define DIALOGB_H

#include <QDialog>

namespace Ui {
    class Dialogb;
}

class Dialogb : public QDialog {
    Q_OBJECT
public:
    Dialogb(QWidget *parent = 0, int task_ad=0);
    ~Dialogb();


private slots:

    void on_pushButton_close_image_clicked();
    void receivetask(int task);

private:
    Ui::Dialogb *ui;

};

#endif // DIALOGB_H
