#ifndef FACEASSESSMENT_H
#define FACEASSESSMENT_H
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\objdetect\objdetect.hpp>
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
	double caculateSymmetry(double*);
	double caculateSymmetry(Mat, double*);
	double caculateSymmetry(Mat, double*, Rect);
	map<int, Mat> caculateSymmetry(map<int, Mat> infoMap, map<int, double*> landMap);
	map<QString, Mat> caculateSharpness(map<QString, Mat>);
	map<QString, Mat> caculateBrightness(map<QString, Mat>);
	Mat rotateImage(Mat, double);
	double* landmarkRotation(double *, double, double, double);

	void setSelectMap(map<QString, vector<Mat>>);
	map<QString, vector<Mat>> getSelectMap();
	void setProgressMin(double);
	void setProgressMax(double);
	void run();

private:
	string face_cascade_name;
	FLANDMARK_Model *landmarkModel;
	double progressMin;
	double progressMax;
	map<QString, vector<Mat>> selectMap;
};


#endif // !FACEASSESSMENT_H
