#include "FaceAssessment.h"

FaceAssessment::FaceAssessment()
{
	face_cascade_name = "Resources/haarcascade_frontalface_alt.xml";
	landmarkModel = flandmark_init("Resources/flandmark_model.dat");
}

FaceAssessment::~FaceAssessment()
{
	delete landmarkModel;
}

vector<Rect> FaceAssessment::faceDetection(Mat image)
{
	CascadeClassifier face_cascade;
	vector<Rect> rect_faces;
	if (!face_cascade.load(face_cascade_name)) {
		printf("Error Loading");
	}
	face_cascade.detectMultiScale(image, rect_faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cvSize(90, 90));
	return rect_faces;
}

double * FaceAssessment::landmarkDetection(Mat image, Rect rect_face)
{
	IplImage *img_grayscale = &IplImage(image);
	double *landmarks = (double*)malloc(2 * landmarkModel->data.options.M * sizeof(double));
	int bbox[] = { rect_face.x,rect_face.y,rect_face.x + rect_face.width,rect_face.y + rect_face.height };
	flandmark_detect(img_grayscale, bbox, landmarkModel, landmarks);
	return landmarks;
}

Rect FaceAssessment::getCropImageBound(double center_x, double center_y, int crop_size)
{
	Rect rect(static_cast<int>(center_x) - crop_size / 2, static_cast<int>(center_y) - crop_size / 2, crop_size, crop_size);
	return rect;
}

double FaceAssessment::getSkewness(double *landmarks)
{
	double mean_x_value = (landmarks[2] + landmarks[4] + landmarks[10] + landmarks[12]) / 4.0;
	double mean_y_value = (landmarks[3] + landmarks[5] + landmarks[11] + landmarks[13]) / 4.0;
	double theta = atan(((landmarks[2] * landmarks[3] + landmarks[4] * landmarks[5] + landmarks[10] * landmarks[11] + landmarks[12] * landmarks[13]) - 4 * mean_x_value*mean_y_value)
		/ (pow(landmarks[2], 2) + pow(landmarks[4], 2) + pow(landmarks[10], 2) + pow(landmarks[12], 2) - 4 * pow(mean_x_value, 2)));
	return theta * 180 / 3.1415926;
}

float FaceAssessment::caculateSymmetry(Mat image)
{
	Mat gray_face_image;
	if (image.channels() == 3) {
		cvtColor(image, gray_face_image, CV_RGB2GRAY);
	}
	else {
		gray_face_image = image;
	}
	float score = 0;
	Mat low_pass_src;
	bilateralFilter(gray_face_image, low_pass_src, 10, 80, 80);
	for (size_t row_num = 0; row_num < gray_face_image.rows; row_num++) {
		for (size_t col_num = 0; col_num < gray_face_image.cols; col_num++) {
			score += abs(gray_face_image.at<uchar>(row_num, col_num) - low_pass_src.at<uchar>(row_num, col_num));
			//cout << "" << endl;
		}
	}
	score = score / (gray_face_image.rows*gray_face_image.cols);
	return  score;
}

float FaceAssessment::caculateSharpness(Mat image)
{
	double score = 0;
	Mat laplacian;
	Mat result_m;
	Mat result_sd;
	Laplacian(image, laplacian, CV_16S, 3);
	convertScaleAbs(laplacian, laplacian);
	meanStdDev(laplacian, result_m, result_sd);
	score = result_sd.at<double>(0, 0);
	return score;
}

map<QString, Mat> FaceAssessment::caculateSymmetry(map<QString, Mat> infoMap)
{
	map<QString, Mat> returnMap;
	map<float, QString> mfs_score;
	map<QString, Mat>::iterator it_info = infoMap.begin();
	float max_score = INT_MIN;
	for (; it_info != infoMap.end(); it_info++) {
		float score = 0;
		QString file_name = it_info->first;
		Mat face_image = it_info->second;
		Mat gray_face_image;
		cvtColor(face_image, gray_face_image, CV_RGB2GRAY);
		score = caculateSymmetry(gray_face_image);
		if (score > max_score) {
			max_score = score;
		}
		mfs_score.insert(make_pair(score, file_name));
	}
	if (infoMap.size() >= 20) {
		map<float, QString>::reverse_iterator it_mfs = mfs_score.rbegin();
		for (size_t i = 0; i < 10; i++, it_mfs++) {
			returnMap.insert(make_pair(it_mfs->second, infoMap.find(it_mfs->second)->second));
		}
	}
	else {
		map<float, QString>::reverse_iterator it_mfs = mfs_score.rbegin();
		for (size_t i = 0; i < infoMap.size()*0.8; i++, it_mfs++) {
			returnMap.insert(make_pair(it_mfs->second, infoMap.find(it_mfs->second)->second));
		}
	}
	return returnMap;
}

map<QString, Mat> FaceAssessment::caculateSharpness(map<QString, Mat> infoMap)
{
	map<QString, Mat> returnMap;
	map<float, QString> mfs_score;
	map<QString, Mat>::iterator it_info = infoMap.begin();
	float max_score = INT_MIN;

	for (; it_info != infoMap.end(); it_info++) {
		double score = 0;
		QString file_name = it_info->first;
		Mat face_image = it_info->second;
		score = caculateSharpness(face_image);
		if (score > max_score) {
			max_score = score;
		}
		mfs_score.insert(make_pair(score, file_name));
	}

	map<float, QString>::iterator it_mfs = mfs_score.begin();
	for (; it_mfs != mfs_score.end(); it_mfs++) {
		if (it_mfs->first / max_score > 0.9) {
			returnMap.insert(make_pair(it_mfs->second, infoMap.find(it_mfs->second)->second));
		}
	}
	return returnMap;
}

map<QString, Mat> FaceAssessment::caculateBrightness(map<QString, Mat> infoMap)
{
	map<QString, Mat> returnMap;
	map<float, QString> mfs_score;
	map<QString, Mat>::iterator it_info = infoMap.begin();
	float max_score = INT_MIN;

	for (; it_info != infoMap.end(); it_info++) {
		float score = 0;
		QString file_name = it_info->first;
		Mat face_image = it_info->second;
		Mat YCbCr_image;
		cvtColor(face_image, YCbCr_image, CV_RGB2YCrCb, 0);
		for (size_t row_num = 0; row_num < YCbCr_image.rows; row_num++) {
			for (size_t col_num = 0; col_num < YCbCr_image.cols; col_num++) {
				Vec3f pixel = YCbCr_image.at<Vec3b>(row_num, col_num);
				score += pixel[0];
			}
		}
		score = score / (YCbCr_image.rows*YCbCr_image.cols);
		if (score > max_score) {
			max_score = score;
		}
		mfs_score.insert(make_pair(score, file_name));
	}

	map<float, QString>::iterator it_mfs = mfs_score.begin();
	//assert(images.size() == vf_img_score.size());
	for (; it_mfs != mfs_score.end(); it_mfs++) {
		if (it_mfs->first / max_score > 0.9) {
			returnMap.insert(make_pair(it_mfs->second, infoMap.find(it_mfs->second)->second));
		}
	}
	return returnMap;
}

Mat FaceAssessment::rotateImage(Mat src, double angle)
{
	Mat dst;
	Point2f pt(src.cols / 2, src.rows / 2);
	Mat r = getRotationMatrix2D(pt, angle, 1.0);
	warpAffine(src, dst, r, Size(src.cols, src.rows));
	return dst;
}

double * FaceAssessment::landmarkRotation(double *landmarks, double angle, double midX, double midY)
{
	float s = sin(angle*3.1415926 / 180);
	float c = cos(angle*3.1415926 / 180);
	double *returnLandmarks = new double[16];
	for (size_t i = 0; i < 15; i += 2) {
		float x = 0;
		float y = 0;
		x = landmarks[i] - midX;
		y = landmarks[i + 1] - midY;
		float xnew = x*c - y*s;
		float ynew = x*s + y*c;
		x = xnew + midX;
		y = ynew + midY;
		returnLandmarks[i] = x;
		returnLandmarks[i + 1] = y;
	}
	return returnLandmarks;
}

void FaceAssessment::setAssessMap(map<QString, map<QString, Mat>> pAssessMap)
{
	assessMap = pAssessMap;
}

map<QString, vector<Mat>> FaceAssessment::getSelectMap()
{
	return selectMap;
}

void FaceAssessment::setProgressMin(double pProgressMin)
{
	progressMin = pProgressMin;
}

void FaceAssessment::setProgressMax(double pProgressMax)
{
	progressMax = pProgressMax;
}

void FaceAssessment::run()
{
	selectMap.clear();
	map<QString, Mat> inputMap;
	map<QString, map<QString, Mat>>::iterator it_assessMap = assessMap.begin();
	for (; it_assessMap != assessMap.end(); it_assessMap++) {
		map<QString, Mat>::iterator it_inputMap = it_assessMap->second.begin();
		for (; it_inputMap != it_assessMap->second.end(); it_inputMap++) {
			
			Mat face_image = it_inputMap->second;
			if (face_image.empty()) {
				cout << "Image read error" << endl;
				continue;
			}
			//faces.swap(vector<Rect>());
			faces = faceDetection(face_image);
			if (faces.empty()) {
				cout << "Face Detection Error" << endl;
				continue;
			}
			Mat gray_image;
			cvtColor(face_image, gray_image, CV_RGB2GRAY);
			double *landmarks = landmarkDetection(gray_image, faces.at(0));
			if (landmarks < 0) {
				cout << "Landmark Detection Error" << endl;
				continue;
			}
			//face_image = face_image(faces.at(0));
			if (face_image.rows >= 100 && face_image.cols >= 100) {
				inputMap.insert(make_pair(it_inputMap->first, face_image));
			}
			cout << "123" << endl;
		}
		map<QString, Mat> outputMap = caculateBrightness(caculateSharpness(caculateSymmetry(inputMap)));
		map<QString, Mat>::iterator it_output = outputMap.begin();
		vector<Mat> imageVector;
		for (; it_output != outputMap.end(); it_output++) {
			imageVector.push_back(it_output->second);
		}
		selectMap.insert(make_pair(it_assessMap->first, imageVector));
		//vector<Mat>().swap(imageVector);
		imageVector.clear();
		inputMap.clear();
		outputMap.clear();
		cout << "456" << endl;
	}
}

