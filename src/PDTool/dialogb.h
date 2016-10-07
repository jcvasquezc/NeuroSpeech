#ifndef DIALOGB_H
#define DIALOGB_H

#include <QDialog>

namespace Ui {
    class Dialogb;
}

class Dialogb : public QDialog {
    Q_OBJECT
public:
    Dialogb(QWidget *parent = 0);
    ~Dialogb();


private slots:
    void on_listWidget_clicked(const QModelIndex &index);

private:
    Ui::Dialogb *ui;

};

#endif // DIALOGB_H
