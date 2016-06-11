#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtCore>
#include <QtWidgets/QMainWindow>
#include <qlabel.h>
#include <qlayout.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qstandarditemmodel.h>
#include "FaceTracking.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void readFile();
	void faceTracking();
	void listItemSelected(const QModelIndex&);

private:
	Ui::MainWindow *ui;
	QWidget *widget;
	QGridLayout *layout;
	QStandardItemModel *stdItemModel;
	int grid_x = 0;
	int grid_y = 0;
	FaceTracking *face_track;
	QMap<QString, QString> fileMap;

};

#endif // MAINWINDOW_H
