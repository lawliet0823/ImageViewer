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
	vector<Rect> faces;
	if (!face_cascade.load(face_cascade_name)) {
		printf("Error Loading");
	}
	face_cascade.detectMultiScale(image, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cvSize(90, 90));
	return faces;
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
	return theta * 180 / 3.14;
}

double FaceAssessment::caculateSymmetry(double *landmarks)
{
	float diff_value = abs(abs(landmarks[14] - landmarks[2]) - abs(landmarks[14] - landmarks[4]));
	return diff_value;
}

double FaceAssessment::caculateSymmetry(Mat image, double *landmarks)
{
	double symmetryValue = 0;
	for (size_t landmark_element = 2; landmark_element < 13; landmark_element += 4) {
		Rect cropRectL = getCropImageBound(static_cast<int>(landmarks[landmark_element]),
			static_cast<int>(landmarks[landmark_element + 1]), 32);
		Rect cropRectR = getCropImageBound(static_cast<int>(landmarks[landmark_element + 2]),
			static_cast<int>(landmarks[landmark_element + 3]), 32);
		Mat crop_ImgL = image(cropRectL);
		Mat crop_ImgR = image(cropRectR);

		// HOG Descriptor
		HOGDescriptor hog(Size(32, 32), Size(16, 16), Size(8, 8), Size(4, 4), 9);
		vector<float> dersL;
		vector<float> dersR;
		vector<Point> locsL;
		vector<Point> locsR;
		hog.compute(crop_ImgL, dersL, Size(4, 4), Size(0, 0), locsL);
		hog.compute(crop_ImgR, dersR, Size(4, 4), Size(0, 0), locsR);
		Mat hogFeatL;
		Mat hogFeatR;
		hogFeatL.create(dersL.size(), 1, CV_32FC1);
		hogFeatR.create(dersR.size(), 1, CV_32FC1);

		for (size_t i = 0; i < dersL.size(); i++) {
			hogFeatL.at<float>(i, 0) = dersL.at(i);
		}
		for (size_t i = 0; i < dersR.size(); i++) {
			hogFeatR.at<float>(i, 0) = dersR.at(i);
		}

		int nHistSize = 65536;
		float fRange[] = { 0.00f, 1.00f };
		const float* fHistRange = { fRange };
		Mat matHistL;
		Mat matHistR;
		calcHist(&hogFeatL, 1, 0, Mat(), matHistL, 1, &nHistSize, &fHistRange);
		calcHist(&hogFeatR, 1, 0, Mat(), matHistR, 1, &nHistSize, &fHistRange);
		symmetryValue += compareHist(matHistL, matHistR, CV_COMP_INTERSECT);
	}
	symmetryValue /= 7;
	return symmetryValue;
}

double FaceAssessment::caculateSymmetry(Mat image, double *landmarks, Rect rect)
{
	image(rect).copyTo(image);
	int crop_x = static_cast<int>(landmarks[0]) - rect.x;
	int crop_y = static_cast<int>(landmarks[1]) - rect.y;

	Rect rImageL(0, 0, crop_x, image.rows);
	Rect rImageR(crop_x, 0, image.cols - crop_x, image.rows);
	Mat imageL;
	Mat imageR;
	image(rImageL).copyTo(imageL);
	image(rImageR).copyTo(imageR);

	int hbins = 30, sbins = 32;
	int channels[] = { 0, 1 };
	int histSize[] = { hbins, sbins };
	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 255 };
	const float *ranges[] = { hranges, sranges };

	Mat patch_HSV;
	Mat histL;
	Mat histR;
	cvtColor(imageL, patch_HSV, CV_BGR2HSV);
	calcHist(&patch_HSV, 1, channels, Mat(), histL, 2, histSize, ranges, true, false);
	normalize(histL, histL, 0, 1, CV_MINMAX);

	cvtColor(imageR, patch_HSV, CV_BGR2HSV);
	calcHist(&patch_HSV, 1, channels, Mat(), histR, 2, histSize, ranges, true, false);
	normalize(histR, histR, 0, 1, CV_MINMAX);

	int numrows = hbins*sbins;

	Mat signL = Mat(numrows, 3, CV_32FC1, Scalar::all(0));
	Mat signR = Mat(numrows, 3, CV_32FC1, Scalar::all(0));

	float value;
	for (size_t h = 0; h < hbins; h++) {
		for (size_t s = 0; s < sbins; s++) {
			value = histL.at<float>(h, s);
			float *dataL = signL.ptr<float>(h*s);
			dataL[0] = value;
			dataL[1] = h;
			dataL[2] = s;

			value = histR.at<float>(h, s);
			float *dataR = signR.ptr<float>(h*s);
			dataR[0] = value;
			dataR[1] = h;
			dataR[2] = s;
		}
	}
	float emd_distance = EMD(signL, signR, CV_DIST_L2);
	return emd_distance;
}

map<int, Mat> FaceAssessment::caculateSymmetry(map<int, Mat> infoMap, map<int, double*> landMap)
{
	map<int, Mat> returnMap;
	map<int, float> mfs_score;
	map<int, Mat>::iterator it_info = infoMap.begin();
	for (; it_info != infoMap.end(); it_info++) {
		float score = 0;
		int label_num = it_info->first;
		Mat face_image = it_info->second;

		double angle = getSkewness(landMap.find(label_num)->second);
		Mat rotation_image = rotateImage(face_image, angle);
		//it_info->second = rotation_image;
		score = caculateSymmetry(rotation_image, landmarkRotation(landMap.find(label_num)->second, -angle, rotation_image.cols / 2, rotation_image.rows / 2));
		mfs_score.insert(make_pair(label_num, score));
	}

	vector<pair<int, float>> vec(mfs_score.begin(), mfs_score.end());
	sort(vec.begin(), vec.end(), CompareByValue());

	for (size_t i = 0; i < 10; i++) {
		returnMap.insert(make_pair(vec.at(i).first, infoMap.find(vec.at(i).first)->second));
	}
	/*
	for (size_t i = 0; i < 10; i++, it_mfs++) {
		returnMap.insert(make_pair(it_mfs->second, infoMap.find(it_mfs->second)->second));
	}
	*/
	return returnMap;
}

map<QString, Mat> FaceAssessment::caculateSharpness(map<QString, Mat> infoMap)
{
	map<QString, Mat> returnMap;
	map<float, QString> mfs_score;

	map<QString, Mat>::iterator it_info = infoMap.begin();
	for (; it_info != infoMap.end(); it_info++) {
		QString file_name = it_info->first;
		Mat face_image = it_info->second;
		Mat grey_image;
		cvtColor(face_image, grey_image, CV_RGB2GRAY);
		float score = 0;
		Mat low_pass_src;
		GaussianBlur(grey_image, low_pass_src, Size(3, 3), 0, 0);
		for (size_t row_num = 0; row_num < grey_image.rows; row_num++) {
			for (size_t col_num = 0; col_num < grey_image.cols; col_num++) {
				score += abs(grey_image.at<uchar>(row_num, col_num) - low_pass_src.at<uchar>(row_num, col_num));
			}
		}
		score = score / (grey_image.rows*grey_image.cols);
		mfs_score.insert(make_pair(score, file_name));
	}
	map<float, QString>::reverse_iterator it_mfs = mfs_score.rbegin();
	for (size_t i = 0; i < 10; i++, it_mfs++) {
		returnMap.insert(make_pair(it_mfs->second, infoMap.find(it_mfs->second)->second));
	}
	return returnMap;
}

map<QString, Mat> FaceAssessment::caculateBrightness(map<QString, Mat> infoMap)
{
	map<QString, Mat> returnMap;
	map<float, QString> mfs_score;

	map<QString, Mat>::iterator it_info = infoMap.begin();
	for (; it_info != infoMap.end(); it_info++) {
		QString file_name = it_info->first;
		Mat face_image = it_info->second;
		Mat grey_image;
		cvtColor(face_image, grey_image, CV_RGB2GRAY);
		float score = 0;
		for (size_t row_num = 0; row_num < grey_image.rows; row_num++) {
			for (size_t col_num = 0; col_num < grey_image.cols; col_num++) {
				score += grey_image.at<uchar>(row_num, col_num);
			}
		}
		score = score / (grey_image.rows*grey_image.cols);
		mfs_score.insert(make_pair(score, file_name));
	}
	map<float, QString>::reverse_iterator it_mfs = mfs_score.rbegin();
	for (size_t i = 0; i < 10; i++, it_mfs++) {
		returnMap.insert(make_pair(it_mfs->second, infoMap.find(it_mfs->second)->second));
	}
	return returnMap;
}

Mat FaceAssessment::rotateImage(Mat src, double angle)
{
	Mat dst;
	Point2f pt(src.cols / 2., src.rows / 2.);
	Mat r = getRotationMatrix2D(pt, -8, 1.0);
	warpAffine(src, dst, r, Size(src.cols, src.rows));
	return dst;
}

double * FaceAssessment::landmarkRotation(double *landmarks, double angle, double midX, double midY)
{
	for (size_t i = 0; i < 15; i += 2) {
		double tempX = 0;
		double tempY = 0;
		landmarks[i] = landmarks[i] - midX;
		landmarks[i + 1] = landmarks[i + 1] - midY;
		tempX = (landmarks[i] * cos(angle*3.1415926 / 180) - landmarks[i + 1] * sin(angle*3.1415926 / 180)) + midX;
		tempY = (landmarks[i] * sin(angle*3.1415926 / 180) + landmarks[i + 1] * cos(angle*3.1415926 / 180)) + midY;
		landmarks[i] = tempX;
		landmarks[i + 1] = tempY;
	}
	return landmarks;
}

void FaceAssessment::setSelectMap(map<QString, vector<Mat>> pSelectMap)
{
	selectMap = pSelectMap;
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
	if (selectMap.size() == 0) {
		return;
	}
	map<QString, vector<Mat>>::iterator it_select = selectMap.begin();
	map<QString, vector<Mat>> returnMap; // final return map
	for (; it_select != selectMap.end(); it_select++) {
		vector<Mat> tempMatVector = it_select->second;
		vector<Mat> selectMatVector;	// final Mat vector
		map<int, Mat> imageMap;			// put into symmetry function
		map<int, double*> landMap;		// put into symmetry function
		int count = 1;
		//cout << tempMatVector.size() << endl;
		for (size_t element = 0; element < tempMatVector.size(); element++) {
			//cout << i << endl;
			Mat face_image = tempMatVector.at(element);
			//imshow("", face_image);
			//waitKey(50);
			if (face_image.empty()) {
				cout << "Read Error!" << endl;
				continue;
			}
			if (face_image.rows > 150 && face_image.cols > 150) {
				Mat grey_face_image;
				cvtColor(face_image, grey_face_image, CV_RGB2GRAY);
				// Face Detection
				vector<Rect> faces = faceDetection(face_image);
				if (faces.empty()) {
					cout << "Face Detect Error!" << endl;
					continue;
				}

				// Landmark detection
				double *landmarks = landmarkDetection(grey_face_image, faces.at(0));
				if (*landmarks < 0) {
					cout << "Landmark Detect Error!" << endl;
					continue;
				}
				imageMap.insert(make_pair(count, face_image));
				landMap.insert(make_pair(count, landmarks));
				++count;
			}
			cout << tempMatVector.size();
		}
		/*
		cout << "end" << endl;
		imageMap = caculateSymmetry(imageMap, landMap);
		map<int, Mat>::iterator it = imageMap.begin();
		for (; it != imageMap.end(); it++) {
			selectMatVector.push_back(it->second);
		}
		returnMap.insert(make_pair(it_select->first, selectMatVector));
		*/
	}
	selectMap = returnMap;
}

