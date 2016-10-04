#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	face_track = new FaceTracking();
	face_assess = new FaceAssessment();
	face_recog = new FaceRecognition();
	ui->setupUi(this);

	// Layout Setup
	image_layout = new QGridLayout;
	image_widget = new QWidget;
	output_layout = new QGridLayout;
	output_widget = new QWidget;
	stdItemModel = new QStandardItemModel(this);

	// Signal and Slot
	QObject::connect(ui->btnFaceTracking, SIGNAL(clicked()), this, SLOT(faceTracking()));
	QObject::connect(ui->btnReadFile, SIGNAL(clicked()), this, SLOT(readFile()));
	QObject::connect(ui->listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(listItemSelected(const QModelIndex&)));
	QObject::connect(ui->btnFaceSelect, SIGNAL(clicked()), this, SLOT(faceAssessment()));	// Not yet
	QObject::connect(ui->btnCreateModel, SIGNAL(clicked()), this, SLOT(createModel()));
	QObject::connect(ui->btnFaceRecognition, SIGNAL(clicked()), this, SLOT(faceRecognition()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::setSelectLabel()
{
	if (image_layout != NULL) {
		QLayoutItem *item;
		while ((item = image_layout->takeAt(0)) != NULL) {
			item->widget()->hide();
			//delete item->widget();
			delete item;
		}
		select_grid_x = 0;
		select_grid_y = 0;
	}

	map<QString, vector<Mat>>::iterator it_assess = selectMap.begin();
	for (; it_assess != selectMap.end(); it_assess++) {
		ImageLabel *imgLabel = new ImageLabel();
		// set ImageLabel signal
		connect(imgLabel, SIGNAL(clicked()), imgLabel, SLOT(setBorderSlot()));
		cout << it_assess->second.size() << endl;

		// ImageLabel Layout Setting
		Mat resize_image;
		cv::resize(it_assess->second.at(0), resize_image, Size(250, 250));
		QPixmap pix = ASM::cvMatToQPixmap(resize_image);
		imgLabel->setPixmap(pix);
		imgLabel->setFixedSize(250, 250);
		image_layout->addWidget(imgLabel, select_grid_x, select_grid_y);
		image_widget->setLayout(image_layout);
		ui->imageScroll->setWidget(image_widget);

		if (select_grid_y < 3)
			select_grid_y++;
		if (select_grid_y == 3) {
			select_grid_y = 0;
			select_grid_x++;
		}
		searchMap.insert(make_pair(imgLabel, it_assess->second));
	}
}

// call Face Tracking Program
void MainWindow::faceTracking() {
	// Read Video File
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Video"));

	// Check File Name is empty or not
	if (filename.size() == 0)
		return;

	// QString to String
	string convertString = filename.toUtf8().constData();
	size_t found = convertString.find_last_of("/\\");
	string dirNameString = convertString.substr(found + 1);
	dirNameString = dirNameString.substr(0, dirNameString.find("."));
	face_track->setVideoName(convertString);
	face_track->setDirName(dirNameString);
	face_track->run();
}

// Read File
void MainWindow::readFile() {
	// Clear List View
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
		//qDebug() << it_dir.fileName();
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
	//int num = 200;
	//int counter = 0;
	//ui->progressBar->setRange(0, num);
	map<QString, map<QString, Mat>> tempMap;
	//map<QString, vector<Mat>> fileMatMap;
	map<QString, QString>::iterator it_mapss = dirMap.begin();
	for (; it_mapss != dirMap.end(); it_mapss++) {
		//vector<Mat> tempMatMap;
		map<QString, Mat> prepareAssessMap;
		QDirIterator it_dir(it_mapss->second);
		while (it_dir.hasNext()) {
			it_dir.next();
			if (it_dir.fileName() == "." || it_dir.fileName() == "..") {
				continue;
			}
			// Read Image
			Mat image = imread(it_dir.filePath().toUtf8().toStdString());
			if (image.empty()) {
				continue;
			}
			prepareAssessMap.insert(make_pair(it_dir.filePath(), image));
			//++counter;
			//ui->progressBar->setValue(counter);
			//tempMatMap.push_back(image);
		}
		tempMap.insert(make_pair(it_mapss->first, prepareAssessMap));
		//fileMatMap.insert(make_pair(it_mapss->first, tempMatMap));
	}
	face_assess->setAssessMap(tempMap);
	face_assess->run();
	selectMap = face_assess->getSelectMap();
	//ui->progressBar->setValue(num);
	cout << "Finish" << endl;
	setSelectLabel();
}

void MainWindow::createModel() {

	// Save Image which user chooses
	vector<Mat> tempSaveMatVec;
	// Random select image to train model depend on user choice
	map<ImageLabel*, vector<Mat>>::iterator it_search = searchMap.begin();
	for (; it_search != searchMap.end(); it_search++) {
		if (it_search->first->isSelected()) {
			tempSaveMatVec.reserve(tempSaveMatVec.size() + it_search->second.size());
			tempSaveMatVec.insert(tempSaveMatVec.end(), it_search->second.begin(), it_search->second.end());
		}
	}
	random_shuffle(tempSaveMatVec.begin(), tempSaveMatVec.end());


	vector<Mat> targetedImage;
	vector<Mat> distinctionImage;
	// Warning message: Training Image is not enough
	if (tempSaveMatVec.size() < 25) {
		QString text = QString("Need %1 Images\nPlease Select More Targeted Person Image or\n change another video")
			.arg(25 - tempSaveMatVec.size());
		QMessageBox msgBox;
		msgBox.setText(text);
		msgBox.exec();
		return;
	}

	else {

		for (size_t i = 0; i < 25; i++) {
			targetedImage.push_back(tempSaveMatVec.at(i));
		}

		//Read other training image
		QDirIterator it_dir("Training_Image_1");
		while (it_dir.hasNext()) {
			it_dir.next();
			//strList.append(it_dir.next());
			if (it_dir.fileName() == "." || it_dir.fileName() == "..") {
				continue;
			}
			Mat image = imread(it_dir.filePath().toUtf8().toStdString(), 0);
			if (!image.empty()) {
				distinctionImage.push_back(image);
			}
		}
		face_recog->train_model(targetedImage, distinctionImage);
	}

	/*
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
	*/
}


void MainWindow::listItemSelected(const QModelIndex &index) {

	if (index.isValid()) {
		if (image_layout != NULL) {
			QLayoutItem *item;
			while ((item = image_layout->takeAt(0)) != NULL) {
				item->widget()->hide();
				//delete item->widget();
				delete item;
			}
			select_grid_x = 0;
			select_grid_y = 0;
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
				image_layout->addWidget(imgLabel, select_grid_x, select_grid_y);
				image_widget->setLayout(image_layout);
				ui->imageScroll->setWidget(image_widget);
				if (select_grid_y < 3)
					select_grid_y++;
				if (select_grid_y == 3) {
					select_grid_y = 0;
					select_grid_x++;
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
				image_layout->addWidget(tempLabelVecor.at(i), select_grid_x, select_grid_y);
				image_widget->setLayout(image_layout);
				ui->imageScroll->setWidget(image_widget);
				//ui->imageScroll->setLayout(layout);
				if (select_grid_y < 3)
					select_grid_y++;
				if (select_grid_y == 3) {
					select_grid_y = 0;
					select_grid_x++;
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
			connect(imgLabel, SIGNAL(clicked()), imgLabel, SLOT(setBorderSlot()));
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

void MainWindow::faceRecognition() {
	face_recog->recognition();
	//vector<string> targetDirString = face_recog->getTargetDirString();
	map<string, vector<Mat>> targetMap = face_recog->getTargetMap();
	map<string, vector<Mat>>::iterator it_target = targetMap.begin();

	if (targetMap.empty()) {
		QMessageBox msg;
		msg.setText("Can't find targeted person!!!");
		msg.exec();
		return;
	}
	else {
		
		if (output_layout != NULL) {
			QLayoutItem *item;
			while ((item = output_layout->takeAt(0)) != NULL) {
				item->widget()->hide();
				delete item;
			}
			output_grid_x = 0;
			output_grid_y = 0;
		}

		for (; it_target != targetMap.end(); it_target++) {
			for (size_t i = 0; i < it_target->second.size(); i++) {
				ImageLabel *imgLabel = new ImageLabel();
				Mat resize_image;
				cv::resize(it_target->second.at(i), resize_image, Size(250, 250));
				QPixmap pix = ASM::cvMatToQPixmap(resize_image);
				QImage temp = pix.toImage();
				QPainter *painter = new QPainter(&temp);
				painter->setPen(Qt::blue);
				painter->setFont(QFont("Arial", 20));
				painter->drawText(temp.rect(), Qt::AlignCenter, QString::fromStdString(it_target->first));
				imgLabel->setPixmap(QPixmap::fromImage(temp));
				imgLabel->setFixedSize(250, 250);
				output_layout->addWidget(imgLabel, output_grid_x, output_grid_y);
				output_widget->setLayout(output_layout);
				ui->outputScroll->setWidget(output_widget);
				if (output_grid_y < 3)
					output_grid_y++;
				if (output_grid_y == 3) {
					output_grid_y = 0;
					output_grid_x++;
				}
			}
		}
	}
}
