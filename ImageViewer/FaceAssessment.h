#ifndef FACEASSESSMENT_H
#define FACEASSESSMENT_H
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <iostream>
#include <algorithm>
#include <map>
#include <qstring.h>
#include "Headers\flandmark_detector.h"
#include "Headers\liblbp.h"

using namespace cv;
using namespace std;

struct CompareByValue {
	bool operator()(const pair<int, float> &lhs, const pair<int, float> &rhs) {
		return lhs.second < rhs.second;
	}
};

class FaceAssessment
{
public:
	FaceAssessment();
	virtual ~FaceAssessment();
	vector<Rect> faceDetection(Mat);
	double *landmarkDetection(Mat, Rect);
	Rect getCropImageBound(double, double, int);
	double getSkewness(double*);
	float caculateSymmetry(Mat);
	float caculateSharpness(Mat image);
	map<QString, Mat> caculateSymmetry(map<QString, Mat>);
	map<QString, Mat> caculateSharpness(map<QString, Mat>);
	map<QString, Mat> caculateBrightness(map<QString, Mat>);
	Mat rotateImage(Mat, double);
	double *landmarkRotation(double *, double, double, double);

	void setAssessMap(map<QString, map<QString, Mat>>);
	map<QString, vector<Mat>> getSelectMap();
	void setProgressMin(double);
	void setProgressMax(double);
	void run();

private:
	string face_cascade_name;
	FLANDMARK_Model *landmarkModel;
	double progressMin;
	double progressMax;
	map<QString, map<QString, Mat>> assessMap;
	map<QString, vector<Mat>> selectMap;
	vector<Rect> faces;
	int total_count;
};


#endif // !FACEASSESSMENT_H
