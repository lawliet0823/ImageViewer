#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QtCore/QtCore>
#include <qlabel.h>
#include <opencv2\core\core.hpp>

using namespace cv;

class ImageLabel : public QLabel
{
	Q_OBJECT

public:
	explicit ImageLabel(QWidget *parent = 0);
	~ImageLabel();
	bool isSelected();
	void setSelected(bool);
	void setImage(Mat);
	Mat getImage();

signals:
	void clicked();

	private slots:
	void setBorderSlot();

protected:
	void mousePressEvent(QMouseEvent *event);

private:
	bool selected;
	Mat image;
};

#endif // IMAGELABEL_H
