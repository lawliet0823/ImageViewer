#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	face_assess = new FaceAssessment();
	face_recog = new FaceRecognition();
	ui->setupUi(this);
	// Layout Setup
	layout = new QGridLayout;
	widget = new QWidget;
	stdItemModel = new QStandardItemModel(this);
	// Signal and Slot
	QObject::connect(ui->btnFaceTracking, SIGNAL(clicked()), this, SLOT(faceTracking()));
	QObject::connect(ui->btnReadFile, SIGNAL(clicked()), this, SLOT(readFile()));
	QObject::connect(ui->listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(listItemSelected(const QModelIndex&)));
	QObject::connect(ui->btnFaceSelect, SIGNAL(clicked()), this, SLOT(faceAssessment()));	// Not yet
	QObject::connect(ui->btnCreateModel, SIGNAL(clicked()), this, SLOT(createModel()));
}

MainWindow::~MainWindow()
{

}

// call Face Tracking Program
void MainWindow::faceTracking() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Video"));
	if (filename.size() == 0)
		return;
	string convertString = filename.toUtf8().constData();
	face_track = new FaceTracking();
	face_track->setVideoName(convertString);
	face_track->run();
}

// Read File
void MainWindow::readFile() {
	// Check listview state
	stdItemModel->removeRows(0, stdItemModel->rowCount());
	QString dirname = QFileDialog::getExistingDirectory(this, tr("Select Clip Directory"));
	QStringList strList;
	QDirIterator it_dir(dirname);
	while (it_dir.hasNext()) {
		it_dir.next();
		//strList.append(it_dir.next());
		if (it_dir.fileName() == "." || it_dir.fileName() == "..") {
			continue;
		}
		dirMap.insert(make_pair(it_dir.fileName(), it_dir.filePath()));
		//qDebug() << it_dir.filePath();
		QStandardItem *item = new QStandardItem(it_dir.fileName());
		stdItemModel->appendRow(item);
	}
	ui->listView->setModel(stdItemModel);
}

void MainWindow::faceAssessment() {
	if (dirMap.size() == 0) {
		QMessageBox meg;
		meg.setText("Please read file first!!!");
		meg.exec();
		return;
	}
	//multimap<QString, Mat> fileMultimap;
	map<QString, vector<Mat>> fileMatMap;
	map<QString, QString>::iterator it_mapss = dirMap.begin();
	for (; it_mapss != dirMap.end(); it_mapss++) {
		vector<Mat> tempMatMap;
		QDirIterator it_dir(it_mapss->second);
		while (it_dir.hasNext()) {
			it_dir.next();
			if (it_dir.fileName() == "." || it_dir.fileName() == "..") {
				continue;
			}
			Mat image = imread(it_dir.filePath().toUtf8().toStdString());
			tempMatMap.push_back(image);
			// make_pair(DirName, image)
			//qDebug() << it_dir.filePath();
		}
		//qDebug() << it_mapss->first << "	" << tempMatMap.size();
		fileMatMap.insert(make_pair(it_mapss->first, tempMatMap));
	}
	
	face_assess->setSelectMap(fileMatMap);
	face_assess->run();
	selectMap = face_assess->getSelectMap();
}

void MainWindow::createModel() {
	// clear vector element
	trainImageVec.swap(vector<Mat>());
	map<QString, vector<ImageLabel*>>::iterator it_label = labelMap.begin();
	for (; it_label != labelMap.end(); it_label++) {
		vector<ImageLabel*> labelVector = it_label->second;
		for (size_t i = 0; i < labelVector.size(); i++) {
			if (labelVector.at(i)->isSelected()) {
				trainImageVec.push_back(labelVector.at(i)->getImage());
			}
		}
	}
	face_recog->setTrainImage(trainImageVec);
	face_recog->train_model(trainImageVec.size());
}

void MainWindow::listItemSelected(const QModelIndex &index) {
	if (index.isValid()) {

		if (layout != NULL) {
			QLayoutItem *item;
			while ((item = layout->takeAt(0)) != NULL) {
				item->widget()->hide();
				//delete item->widget();
				delete item;
			}
			grid_x = 0;
			grid_y = 0;
		}

		//qDebug() << index.data().toString();
		// Checking system label  
		if (labelMap.find(index.data().toString()) == labelMap.end()) {
			qDebug() << index.data().toString();
			if (selectMap.find(index.data().toString()) == selectMap.end()) {
				return;
			}
			vector<ImageLabel*> labelVecor;
			vector<Mat> selectVector = selectMap.find(index.data().toString())->second;
			for (size_t i = 0; i < selectVector.size(); i++) {
				ImageLabel *imgLabel = new ImageLabel();
				connect(imgLabel, SIGNAL(clicked()), imgLabel, SLOT(setBorderSlot()));
				imgLabel->setImage(selectVector.at(i));
				QPixmap pix = ASM::cvMatToQPixmap(selectVector.at(i));
				imgLabel->setPixmap(pix);
				imgLabel->setFixedSize(250, 250);
				labelVecor.push_back(imgLabel);
				layout->addWidget(imgLabel, grid_x, grid_y);
				widget->setLayout(layout);
				ui->imageScroll->setWidget(widget);
				if (grid_y < 3)
					grid_y++;
				if (grid_y == 3) {
					grid_y = 0;
					grid_x++;
				}
			}
			labelMap.insert(make_pair(index.data().toString(), labelVecor));
			qDebug() << labelMap.size();
		}
		else {
			vector<ImageLabel*> tempLabelVecor = labelMap.find(index.data().toString())->second;
			//qDebug() << labelVecor.at(0) << " else";
			for (size_t i = 0; i < tempLabelVecor.size(); i++) {
				tempLabelVecor.at(i)->show();
				layout->addWidget(tempLabelVecor.at(i), grid_x, grid_y);
				widget->setLayout(layout);
				ui->imageScroll->setWidget(widget);
				//ui->imageScroll->setLayout(layout);
				if (grid_y < 3)
					grid_y++;
				if (grid_y == 3) {
					grid_y = 0;
					grid_x++;
				}
			}
		}
		/*
		QStringList strList;
		QDirIterator it_dir(dirMap.find(index.data().toString())->second);
		while (it_dir.hasNext()) {
			strList.append(it_dir.next());
		}
		for (size_t i = 0; i < strList.size(); i++) {
			ImageLabel *imgLabel = new ImageLabel();
			//QLabel *qlabel = new QLabel(this);
			// Set Signal and SLOT
			connect(imgLabel, SIGNAL(clicked()), imgLabel, SLOT(setBorder()));
			QString string_item = static_cast<QString>(strList.at(i));

			if (string_item.contains(index.data().toString() + "/.") || string_item.contains(index.data().toString() + "/..")) {
				continue;
			}

			QPixmap pix(string_item);
			imgLabel->setPixmap(pix);
			imgLabel->setFixedSize(250, 250);
			layout->addWidget(imgLabel, grid_x, grid_y);
			widget->setLayout(layout);
			ui->imageScroll->setWidget(widget);
			if (grid_y < 3)
				grid_y++;
			if (grid_y == 3) {
				grid_y = 0;
				grid_x++;
			}
		}
		*/
	}

}
