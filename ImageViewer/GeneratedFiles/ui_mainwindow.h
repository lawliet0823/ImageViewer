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
    QPushButton *btnFaceRecognition;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QScrollArea *outputScroll;
    QWidget *scrollAreaWidgetContents_2;
    QLabel *label_4;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1116, 867);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        imageScroll = new QScrollArea(centralWidget);
        imageScroll->setObjectName(QStringLiteral("imageScroll"));
        imageScroll->setGeometry(QRect(30, 40, 831, 421));
        imageScroll->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 829, 419));
        imageScroll->setWidget(scrollAreaWidgetContents);
        listView = new QListView(centralWidget);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setGeometry(QRect(890, 70, 191, 281));
        btnReadFile = new QPushButton(centralWidget);
        btnReadFile->setObjectName(QStringLiteral("btnReadFile"));
        btnReadFile->setGeometry(QRect(910, 470, 161, 61));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(12);
        font.setBold(false);
        font.setWeight(50);
        btnReadFile->setFont(font);
        btnFaceTracking = new QPushButton(centralWidget);
        btnFaceTracking->setObjectName(QStringLiteral("btnFaceTracking"));
        btnFaceTracking->setGeometry(QRect(910, 390, 161, 61));
        btnFaceTracking->setFont(font);
        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(150, 780, 711, 31));
        progressBar->setValue(0);
        btnFaceSelect = new QPushButton(centralWidget);
        btnFaceSelect->setObjectName(QStringLiteral("btnFaceSelect"));
        btnFaceSelect->setGeometry(QRect(910, 550, 161, 61));
        btnFaceSelect->setFont(font);
        btnCreateModel = new QPushButton(centralWidget);
        btnCreateModel->setObjectName(QStringLiteral("btnCreateModel"));
        btnCreateModel->setGeometry(QRect(910, 630, 161, 61));
        btnCreateModel->setFont(font);
        btnFaceRecognition = new QPushButton(centralWidget);
        btnFaceRecognition->setObjectName(QStringLiteral("btnFaceRecognition"));
        btnFaceRecognition->setGeometry(QRect(910, 710, 161, 61));
        btnFaceRecognition->setFont(font);
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 780, 101, 31));
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        font1.setPointSize(16);
        font1.setBold(true);
        font1.setItalic(false);
        font1.setWeight(75);
        label->setFont(font1);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 10, 181, 21));
        QFont font2;
        font2.setFamily(QStringLiteral("Arial"));
        font2.setPointSize(12);
        font2.setBold(true);
        font2.setWeight(75);
        label_2->setFont(font2);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 472, 121, 20));
        label_3->setFont(font2);
        outputScroll = new QScrollArea(centralWidget);
        outputScroll->setObjectName(QStringLiteral("outputScroll"));
        outputScroll->setGeometry(QRect(30, 500, 831, 271));
        outputScroll->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QStringLiteral("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 829, 269));
        outputScroll->setWidget(scrollAreaWidgetContents_2);
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(890, 40, 211, 21));
        label_4->setFont(font2);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1116, 22));
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
        btnFaceRecognition->setText(QApplication::translate("MainWindow", "Face Recognition", 0));
        label->setText(QApplication::translate("MainWindow", "Progress:", 0));
        label_2->setText(QApplication::translate("MainWindow", "Image Display Window", 0));
        label_3->setText(QApplication::translate("MainWindow", "Result Output", 0));
        label_4->setText(QApplication::translate("MainWindow", "Directory and File Display", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
