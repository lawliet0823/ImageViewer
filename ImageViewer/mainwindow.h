#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtCore>
#include <QtWidgets/QMainWindow>
#include <qlabel.h>
#include <qlayout.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qstandarditemmodel.h>
#include <ImageLabel.h>
#include <map>
#include "FaceTracking.h"
#include "FaceAssessment.h"
#include "FaceRecognition.h"
#include "ImageConvert.h"

using namespace std;

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void setSelectLabel();

	private slots:
	void faceTracking();
	void faceAssessment();
	void faceRecognition();
	void createModel();
	void readFile();
	void listItemSelected(const QModelIndex&);

private:
	Ui::MainWindow *ui;
	QWidget *widget;
	QGridLayout *layout;
	QStandardItemModel *stdItemModel;
	int grid_x = 0;
	int grid_y = 0;
	FaceTracking *face_track;
	FaceAssessment *face_assess;
	FaceRecognition *face_recog;
	// Directory Name, Directory Path
	map<QString, QString> dirMap;
	// Directory Name, Image Vector
	map<QString, vector<Mat>> selectMap;

	map<ImageLabel*, vector<Mat>> searchMap;

	map<QString, vector<ImageLabel*>> labelMap;
	vector<Mat> trainImageVec;
};

#endif // MAINWINDOW_H
