/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QScrollArea *imageScroll;
    QWidget *scrollAreaWidgetContents;
    QListView *listView;
    QPushButton *btnReadFile;
    QPushButton *btnFaceTracking;
    QProgressBar *progressBar;
    QPushButton *btnFaceSelect;
    QPushButton *btnCreateModel;
    QPushButton *pushButton;
    QLabel *label;
    QScrollArea *timeScroll;
    QWidget *scrollAreaWidgetContents_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1089, 799);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        imageScroll = new QScrollArea(centralWidget);
        imageScroll->setObjectName(QStringLiteral("imageScroll"));
        imageScroll->setGeometry(QRect(30, 30, 781, 401));
        imageScroll->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 779, 399));
        imageScroll->setWidget(scrollAreaWidgetContents);
        listView = new QListView(centralWidget);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setGeometry(QRect(860, 30, 181, 271));
        btnReadFile = new QPushButton(centralWidget);
        btnReadFile->setObjectName(QStringLiteral("btnReadFile"));
        btnReadFile->setGeometry(QRect(870, 400, 161, 61));
        btnFaceTracking = new QPushButton(centralWidget);
        btnFaceTracking->setObjectName(QStringLiteral("btnFaceTracking"));
        btnFaceTracking->setGeometry(QRect(870, 320, 161, 61));
        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(140, 690, 651, 31));
        progressBar->setValue(0);
        btnFaceSelect = new QPushButton(centralWidget);
        btnFaceSelect->setObjectName(QStringLiteral("btnFaceSelect"));
        btnFaceSelect->setGeometry(QRect(870, 480, 161, 61));
        btnCreateModel = new QPushButton(centralWidget);
        btnCreateModel->setObjectName(QStringLiteral("btnCreateModel"));
        btnCreateModel->setGeometry(QRect(870, 560, 161, 61));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(870, 640, 161, 61));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(50, 690, 81, 31));
        QFont font;
        font.setPointSize(16);
        label->setFont(font);
        timeScroll = new QScrollArea(centralWidget);
        timeScroll->setObjectName(QStringLiteral("timeScroll"));
        timeScroll->setGeometry(QRect(30, 490, 781, 121));
        timeScroll->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QStringLiteral("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 779, 119));
        timeScroll->setWidget(scrollAreaWidgetContents_2);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1089, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        btnReadFile->setText(QApplication::translate("MainWindow", "Read File", 0));
        btnFaceTracking->setText(QApplication::translate("MainWindow", "Face Tracking", 0));
        btnFaceSelect->setText(QApplication::translate("MainWindow", "Face Assessment", 0));
        btnCreateModel->setText(QApplication::translate("MainWindow", "Create Model", 0));
        pushButton->setText(QApplication::translate("MainWindow", "Face Retrievel", 0));
        label->setText(QApplication::translate("MainWindow", "Progress:", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
