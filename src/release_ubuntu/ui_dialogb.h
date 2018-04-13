/********************************************************************************
** Form generated from reading UI file 'dialogb.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGB_H
#define UI_DIALOGB_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialogb
{
public:
    QLabel *label_title;
    QLabel *label_image;
    QPushButton *pushButton_close_image;
    QLabel *label_time;

    void setupUi(QDialog *Dialogb)
    {
        if (Dialogb->objectName().isEmpty())
            Dialogb->setObjectName(QStringLiteral("Dialogb"));
        Dialogb->resize(1423, 693);
        label_title = new QLabel(Dialogb);
        label_title->setObjectName(QStringLiteral("label_title"));
        label_title->setGeometry(QRect(550, 10, 81, 16));
        QFont font;
        font.setPointSize(11);
        label_title->setFont(font);
        label_image = new QLabel(Dialogb);
        label_image->setObjectName(QStringLiteral("label_image"));
        label_image->setGeometry(QRect(270, 0, 901, 601));
        label_image->setFont(font);
        pushButton_close_image = new QPushButton(Dialogb);
        pushButton_close_image->setObjectName(QStringLiteral("pushButton_close_image"));
        pushButton_close_image->setGeometry(QRect(680, 650, 89, 25));
        label_time = new QLabel(Dialogb);
        label_time->setObjectName(QStringLiteral("label_time"));
        label_time->setGeometry(QRect(550, 30, 41, 31));
        QFont font1;
        font1.setPointSize(18);
        label_time->setFont(font1);
        label_time->raise();
        label_title->raise();
        pushButton_close_image->raise();
        label_image->raise();

        retranslateUi(Dialogb);

        QMetaObject::connectSlotsByName(Dialogb);
    } // setupUi

    void retranslateUi(QDialog *Dialogb)
    {
        Dialogb->setWindowTitle(QApplication::translate("Dialogb", "Dialog", nullptr));
        label_title->setText(QApplication::translate("Dialogb", "Title", nullptr));
        label_image->setText(QString());
        pushButton_close_image->setText(QApplication::translate("Dialogb", "Close", nullptr));
        label_time->setText(QApplication::translate("Dialogb", "60", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialogb: public Ui_Dialogb {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGB_H
