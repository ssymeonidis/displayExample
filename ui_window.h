/********************************************************************************
** Form generated from reading UI file 'window.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "glWidget.h"

QT_BEGIN_NAMESPACE

class Ui_window
{
public:
    QWidget *centralWidget;
    glWidget *glWidgetQT;
    QGroupBox *maskBox;
    QRadioButton *mask1;
    QRadioButton *mask2;
    QRadioButton *mask3;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *window)
    {
        if (window->objectName().isEmpty())
            window->setObjectName(QStringLiteral("window"));
        window->resize(818, 570);
        centralWidget = new QWidget(window);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        glWidgetQT = new glWidget(centralWidget);
        glWidgetQT->setObjectName(QStringLiteral("glWidgetQT"));
        glWidgetQT->setGeometry(QRect(10, -10, 640, 512));
        maskBox = new QGroupBox(centralWidget);
        maskBox->setObjectName(QStringLiteral("maskBox"));
        maskBox->setGeometry(QRect(660, 10, 141, 121));
        mask1 = new QRadioButton(maskBox);
        mask1->setObjectName(QStringLiteral("mask1"));
        mask1->setGeometry(QRect(10, 30, 117, 22));
        mask2 = new QRadioButton(maskBox);
        mask2->setObjectName(QStringLiteral("mask2"));
        mask2->setGeometry(QRect(10, 60, 117, 22));
        mask3 = new QRadioButton(maskBox);
        mask3->setObjectName(QStringLiteral("mask3"));
        mask3->setGeometry(QRect(10, 90, 117, 22));
        mask3->setChecked(true);
        window->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(window);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 818, 25));
        window->setMenuBar(menuBar);
        mainToolBar = new QToolBar(window);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        window->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(window);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        window->setStatusBar(statusBar);

        retranslateUi(window);
        QObject::connect(mask1, SIGNAL(clicked()), window, SLOT(updateMask()));
        QObject::connect(mask2, SIGNAL(clicked()), window, SLOT(updateMask()));
        QObject::connect(mask3, SIGNAL(clicked()), window, SLOT(updateMask()));

        QMetaObject::connectSlotsByName(window);
    } // setupUi

    void retranslateUi(QMainWindow *window)
    {
        window->setWindowTitle(QApplication::translate("window", "window", 0));
        maskBox->setTitle(QApplication::translate("window", "Display Options", 0));
        mask1->setText(QApplication::translate("window", "no mask", 0));
        mask2->setText(QApplication::translate("window", "full mask", 0));
        mask3->setText(QApplication::translate("window", "red mask", 0));
    } // retranslateUi

};

namespace Ui {
    class window: public Ui_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOW_H
