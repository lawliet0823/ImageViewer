#include "ImageLabel.h"

ImageLabel::ImageLabel(QWidget *parent)
	: QLabel(parent) {
	selected = false;
}

ImageLabel::~ImageLabel() {

}

bool ImageLabel::isSelected() {
	return selected;
}

void ImageLabel::setSelected(bool pSelected) {
	selected = pSelected;
}

void ImageLabel::setImage(Mat pImage) {
	image = pImage;
}

void ImageLabel::setDisplayText(QString text) {
	displayText = text;
}

Mat ImageLabel::getImage() {
	return image;
}

void ImageLabel::setBorderSlot() {
	if (selected) {
		this->setStyleSheet("");
		selected = false;
	}
	else {
		this->setStyleSheet("QLabel {border-style: solid; border-width: 5px; border-color: red;}");
		selected = true;
	}
}

void ImageLabel::mousePressEvent(QMouseEvent *event) {
	emit clicked();
}

/*
void ImageLabel::paintEvent(QPaintEvent * event) {
	
	QPainter painter(this);
	painter.setPen(QColor(0, 160, 230));
	QFont font;
	font.setPointSize(30);
	painter.setFont(font);
	painter.drawText(rect(), Qt::AlignCenter, displayText);
}

*/
