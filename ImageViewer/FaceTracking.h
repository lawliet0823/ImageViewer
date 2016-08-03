#ifndef FACETRACKING_H
#define FACETRACKING_H
#include "Headers\TLD.h"
#include "Headers\tld_utils.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <iostream>
#include <direct.h>
#include <sys\stat.h>

using namespace cv;

struct TLD_Info {
	TLD *tld;
	int dirCounter;
	int fileCounter;
	int remainFrame;
};

class FaceTracking {

public:
	FaceTracking();
	~FaceTracking();
	vector<Rect> faceDetection(Mat);
	void createDirectory(VideoCapture &, FileStorage &, Mat &, vector<Rect> &, vector<TLD_Info> &, int &, double &, string);
	void setDirName(string);
	void setMaxValue(double);
	void setVideoName(string);
	void setTLD_Info(TLD_Info&, TLD*, int, int, int);
	void freeTLD_Info(vector<TLD_Info> &);
	void run();

private:
	double max_value;
	string face_cascade_name;
	string videoName;
	string dirName;
	vector<Rect> faces;
};


#endif // !FACETRACKING.h
