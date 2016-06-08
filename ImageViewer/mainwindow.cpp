#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	// Layout Setup
	layout = new QGridLayout;
	widget = new QWidget;
	stdItemModel = new QStandardItemModel(this);
	// Signal and Slot
	QObject::connect(ui->btnReadFile, SIGNAL(clicked()), this, SLOT(readFile()));
	QObject::connect(ui->btnLoadImage, SIGNAL(clicked()), this, SLOT(loadImage()));
	QObject::connect(ui->listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(listItemSelected(const QModelIndex&)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::readFile() {
	// Check listview state
	QString dirname = QFileDialog::getExistingDirectory(this, tr("Select Clip Directory"));
	QStringList strList;
	QDirIterator it_dir(dirname);
	while (it_dir.hasNext()) {
		strList.append(it_dir.next());
	}
	for (size_t i = 0; i < strList.size(); i++) {
		QString string_item = static_cast<QString>(strList.at(i));
		if (string_item.contains(dirname + "/.") || string_item.contains(dirname + "/..")) {
			continue;
		}
		QStandardItem *item = new QStandardItem(string_item);
		stdItemModel->appendRow(item);
	}
	ui->listView->setModel(stdItemModel);

}

void MainWindow::loadImage() {

}

void MainWindow::listItemSelected(const QModelIndex &index) {
	if (index.isValid()) {
		if (layout != NULL) {
			QLayoutItem *item;
			while ((item = layout->takeAt(0)) != NULL) {
				delete item->widget();
				delete item;
			}
			grid_x = 0;
			grid_y = 0;
		}
		QStringList strList;
		QDirIterator it_dir(index.data().toString());
		while (it_dir.hasNext()) {
			strList.append(it_dir.next());
		}
		for (size_t i = 0; i < strList.size(); i++) {
			QLabel *qlabel = new QLabel(this);
			QString string_item = static_cast<QString>(strList.at(i));
			if (string_item.contains(index.data().toString() + "/.") || string_item.contains(index.data().toString() + "/..")) {
				continue;
			}
			QPixmap pix(string_item);
			qlabel->setPixmap(pix);
			qlabel->setFixedSize(250, 250);
			layout->addWidget(qlabel, grid_x, grid_y);
			widget->setLayout(layout);
			ui->scrollArea->setWidget(widget);
			if (grid_y < 3)
				grid_y++;
			if (grid_y == 3) {
				grid_y = 0;
				grid_x++;
			}
		}
	}

}