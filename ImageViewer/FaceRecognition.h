#ifndef FACERECOGNITION_H
#define FACERECOGNITION_H

#include <iostream>
#include <iterator>
#include <fstream>

#include <QtCore/QtCore>
#include <qmessagebox.h>

#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

#include <Headers\flandmark_detector.h>
#include <Headers\lbp.hpp>
#include <Headers\histogram.hpp>
#include <Headers\svm.h>


#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#define min(a,b) (a) > (b) ? b : a

using namespace cv;
using namespace lbp;
using namespace std;

class FaceRecognition
{
public:
	FaceRecognition();
	virtual ~FaceRecognition();

	Mat lbpFeature(Mat);
	Mat rotateImage(Mat src, double angle);
	Rect getCropImageBound(double, double, int);
	vector<Rect> faceDetection(Mat);
	vector<string> getTargetDirString();
	double getSkewness(double *landmarks);
	double* landmarkDetection(Mat, Rect);
	double* landmarkRotation(double *landmarks, double angle, double midX, double midY);
	inline bool isImgOutofBound(int, int, int, int, int);
	void train_model(vector<Mat> posVector, vector<Mat> negVector);
	void recognition();
	void setTrainImage(vector<Mat>);
	Mat getFeatureVector(Mat face_image);
	inline svm_node* getSVM_Node(Mat face_image);

private:
	string face_cascade_name;
	FLANDMARK_Model *landmarkModel;
	int train_num;

	// SVM setup
	struct svm_parameter _param;
	struct svm_problem _prob;

	vector<Mat> trainImage;
	vector<Rect> faces;
	vector<string> targetDirString;
};

#endif // ! FACERECOGNITION_H


