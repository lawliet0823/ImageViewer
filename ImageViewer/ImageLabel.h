#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <qlabel.h>

class ImageLabel : public QLabel
{
	Q_OBJECT

public:
	explicit ImageLabel(QWidget *parent = 0);
	~ImageLabel();
	bool isSelected();
	void setSelected(bool);

signals:
	void clicked();

private slots:
	void setBorder();

protected:
	void mousePressEvent(QMouseEvent *event);

private:
	bool selected;
};

#endif // IMAGELABEL_H
