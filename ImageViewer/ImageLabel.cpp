#include "ImageLabel.h"

ImageLabel::ImageLabel(QWidget *parent)
	: QLabel(parent)
{
	selected = false;
}

ImageLabel::~ImageLabel()
{

}

bool ImageLabel::isSelected() {
	return selected;
}

void ImageLabel::setSelected(bool pSelected) {
	selected = pSelected;
}

void ImageLabel::setBorder() {
	if (selected) {
		this->setStyleSheet("");
		selected = false;
	}
	else {
		this->setStyleSheet("QLabel {border-style: solid; border-width: 5px; border-color: green;}");
		selected = true;
	}
}

void ImageLabel::mousePressEvent(QMouseEvent *event) {
	emit clicked();
}
