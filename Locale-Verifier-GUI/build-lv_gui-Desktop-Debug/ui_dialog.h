/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QGridLayout *gridLayout;
    QCheckBox *checkBox;
    QTextBrowser *textBrowser;
    QFormLayout *formLayout_3;
    QComboBox *comboBox;
    QLabel *label;
    QLabel *stateLabel;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(400, 225);
        Dialog->setMinimumSize(QSize(380, 180));
        QFont font;
        font.setPointSize(9);
        Dialog->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral("../resources/dict.png"), QSize(), QIcon::Normal, QIcon::Off);
        Dialog->setWindowIcon(icon);
        gridLayout = new QGridLayout(Dialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        checkBox = new QCheckBox(Dialog);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        QFont font1;
        font1.setPointSize(8);
        checkBox->setFont(font1);

        gridLayout->addWidget(checkBox, 0, 3, 1, 1, Qt::AlignRight);

        textBrowser = new QTextBrowser(Dialog);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));

        gridLayout->addWidget(textBrowser, 2, 1, 1, 3);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        comboBox = new QComboBox(Dialog);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QStringLiteral("comboBox"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, comboBox);

        label = new QLabel(Dialog);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font1);

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label);


        gridLayout->addLayout(formLayout_3, 0, 1, 1, 2);

        stateLabel = new QLabel(Dialog);
        stateLabel->setObjectName(QStringLiteral("stateLabel"));
        stateLabel->setFont(font1);

        gridLayout->addWidget(stateLabel, 3, 1, 1, 1);

        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        QFont font2;
        font2.setPointSize(10);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setUnderline(false);
        font2.setWeight(50);
        font2.setStrikeOut(false);
        buttonBox->setFont(font2);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Close);
        buttonBox->setCenterButtons(false);

        gridLayout->addWidget(buttonBox, 3, 2, 1, 2);


        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Locale Verifier (Dil Do\304\237rulay\304\261c\304\261)", nullptr));
        checkBox->setText(QApplication::translate("Dialog", "Verify (Do\304\237rula)", nullptr));
        textBrowser->setHtml(QApplication::translate("Dialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /><span style=\" font-weight:600;\">(EN)</span> Choose an option from above.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">System language will be chaned to <span style=\" font-weight:600;\">English</span>.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-ind"
                        "ent:0px;\"><span style=\" font-weight:600;\">(TR)</span> Yukar\304\261dan bir se\303\247enek se\303\247iniz.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Sistem dili <span style=\" font-weight:600;\">\304\260ngilizce</span>'ye \303\247evrilecektir.</p></body></html>", nullptr));
        comboBox->setItemText(0, QApplication::translate("Dialog", "English (\304\260ngilizce)", nullptr));
        comboBox->setItemText(1, QApplication::translate("Dialog", "Turkish (T\303\274rk\303\247e)", nullptr));

        label->setText(QApplication::translate("Dialog", "Language (Dil):", nullptr));
        stateLabel->setText(QApplication::translate("Dialog", "Ready (Haz\304\261r)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
