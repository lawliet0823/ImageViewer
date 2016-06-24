#ifndef FACETRACKING_H
#define FACETRACKING_H
#include "Headers\TLD.h"
#include "Headers\tld_utils.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\objdetect\objdetect.hpp>
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
	void createDirectory(VideoCapture &, FileStorage &, Mat &, vector<Rect> &, vector<TLD_Info> &, int &, double &, string);
	void setDirName(string);
	void setMaxValue(double);
	void setVideoName(string);
	void setTLD_Info(TLD_Info&, TLD*, int, int, int);
	void setVideoCapture(String);
	void freeTLD_Info(vector<TLD_Info> &);
	void run();
	vector<Rect> faceDetection(Mat);

private:
	double max_value;
	string face_cascade_name;
	string videoName;
	string dirName;
};


#endif // !FACETRACKING.h
