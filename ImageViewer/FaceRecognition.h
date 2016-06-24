#ifndef FACERECOGNITION_H
#define FACERECOGNITION_H

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
	Rect getCropImageBound(double, double, int);
	vector<Rect> faceDetection(Mat);
	double* landmarkDetection(Mat, Rect);
	Mat lbpFeature(Mat);
	inline bool isImgOutofBound(int, int, int, int , int);
	void train_model(int);
	void recognition();

private:
	String face_cascade_name;
	FLANDMARK_Model *landmarkModel;
	int feature_total_num;
	int cell_size;
	int train_num;

	// SVM setup
	struct svm_parameter _param;
	struct svm_problem _prob;

	vector<Mat> trainImage;
};

#endif // ! FACERECOGNITION_H


