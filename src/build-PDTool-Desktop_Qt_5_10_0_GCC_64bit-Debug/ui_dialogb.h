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
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_Dialogb
{
public:
    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit;
    QTextEdit *textEdit_2;
    QLabel *label;
    QLabel *label_2;
    QListWidget *listWidget;

    void setupUi(QDialog *Dialogb)
    {
        if (Dialogb->objectName().isEmpty())
            Dialogb->setObjectName(QStringLiteral("Dialogb"));
        Dialogb->resize(442, 184);
        buttonBox = new QDialogButtonBox(Dialogb);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(230, 140, 181, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        textEdit = new QTextEdit(Dialogb);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(100, 20, 111, 31));
        textEdit_2 = new QTextEdit(Dialogb);
        textEdit_2->setObjectName(QStringLiteral("textEdit_2"));
        textEdit_2->setGeometry(QRect(100, 60, 111, 31));
        label = new QLabel(Dialogb);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 30, 81, 16));
        QFont font;
        font.setPointSize(11);
        label->setFont(font);
        label_2 = new QLabel(Dialogb);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 70, 81, 16));
        label_2->setFont(font);
        listWidget = new QListWidget(Dialogb);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(240, 20, 171, 111));

        retranslateUi(Dialogb);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialogb, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialogb, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialogb);
    } // setupUi

    void retranslateUi(QDialog *Dialogb)
    {
        Dialogb->setWindowTitle(QApplication::translate("Dialogb", "Dialog", nullptr));
        label->setText(QApplication::translate("Dialogb", "First Name", nullptr));
        label_2->setText(QApplication::translate("Dialogb", "Last Name", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialogb: public Ui_Dialogb {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGB_H
