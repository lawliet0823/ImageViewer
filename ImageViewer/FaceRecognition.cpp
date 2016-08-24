#include "FaceRecognition.h"

FaceRecognition::FaceRecognition()
{
	face_cascade_name = "Resources/haarcascade_frontalface_alt.xml";
	landmarkModel = flandmark_init("Resources/flandmark_model.dat");

	// SVM parameter setup
	_param.svm_type = C_SVC;
	_param.kernel_type = LINEAR;
	_param.degree = 3;
	_param.gamma = 0.0001;			// 1/num_features
	_param.coef0 = 0;
	_param.nu = 0.5;
	_param.cache_size = 100;
	_param.C = 1;
	_param.eps = 1e-3;
	_param.p = 0.1;
	_param.shrinking = 1;
	_param.probability = 0;
	_param.nr_weight = 0;
	_param.weight_label = NULL;
	_param.weight = NULL;
}


FaceRecognition::~FaceRecognition()
{
}

Rect FaceRecognition::getCropImageBound(double center_x, double center_y, int crop_size)
{
	Rect rect(static_cast<int>(center_x) - crop_size / 2, static_cast<int>(center_y) - crop_size / 2, crop_size, crop_size);
	return rect;
}

vector<Rect> FaceRecognition::faceDetection(Mat image)
{
	CascadeClassifier face_cascade;
	vector<Rect> faces;

	if (!face_cascade.load(face_cascade_name)) {
		printf("Error Loading");
	}
	face_cascade.detectMultiScale(image, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cvSize(90, 90));
	return faces;
}

vector<string> FaceRecognition::getTargetDirString() {
	return targetDirString;
}

double FaceRecognition::getSkewness(double * landmarks) {
	double mean_x_value = (landmarks[2] + landmarks[4] + landmarks[10] + landmarks[12]) / 4.0;
	double mean_y_value = (landmarks[3] + landmarks[5] + landmarks[11] + landmarks[13]) / 4.0;
	double theta = atan(((landmarks[2] * landmarks[3] + landmarks[4] * landmarks[5] + landmarks[10] * landmarks[11] + landmarks[12] * landmarks[13]) - 4 * mean_x_value*mean_y_value)
		/ (pow(landmarks[2], 2) + pow(landmarks[4], 2) + pow(landmarks[10], 2) + pow(landmarks[12], 2) - 4 * pow(mean_x_value, 2)));
	return theta * 180 / 3.1415926;
}

double * FaceRecognition::landmarkDetection(Mat image, Rect rect_face)
{
	IplImage *img_grayscale = &IplImage(image);
	//IplImage *img_grayscale = cvCreateImage(cvSize(i_image->width, i_image->height), IPL_DEPTH_8U, 1);
	//cvCvtColor(i_image,img_grayscale,CV_BGR2GRAY);
	double *landmarks = (double*)malloc(2 * landmarkModel->data.options.M * sizeof(double));
	int bbox[] = { rect_face.x,rect_face.y,rect_face.x + rect_face.width,rect_face.y + rect_face.height };
	flandmark_detect(img_grayscale, bbox, landmarkModel, landmarks);
	return landmarks;
}

double * FaceRecognition::landmarkRotation(double * landmarks, double angle, double midX, double midY) {
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

Mat FaceRecognition::lbpFeature(Mat image)
{
	Mat dst;
	GaussianBlur(image, dst, Size(5, 5), 5, 3, BORDER_CONSTANT);
	//Mat lbpImage = OLBP(dst);
	Mat lbpImage = ELBP(dst, 2, 8);
	return lbpImage;
}

Mat FaceRecognition::rotateImage(Mat src, double angle) {
	Mat dst;
	Point2f pt(src.cols / 2, src.rows / 2);
	Mat r = getRotationMatrix2D(pt, angle, 1.0);
	warpAffine(src, dst, r, Size(src.cols, src.rows));
	return dst;
}

inline bool FaceRecognition::isImgOutofBound(int x, int y, int crop_size, int img_width, int img_height)
{
	if (x - crop_size / 2 < 0 || x + crop_size / 2 > img_width || y - crop_size / 2 < 0 || y + crop_size / 2 > img_height)
		return true;
	else
		return false;
}

void FaceRecognition::train_model(vector<Mat> posVector, vector<Mat> negVector) {

	int cell_size = 36;
	int feature_total_num = 3 * 7 * 16 * 59 + 1;
	int train_num_count = 0;

	// SVM Setup
	struct svm_parameter _param;
	struct svm_problem _prob;

	// training sample number
	_prob.l = posVector.size() + negVector.size();
	_prob.y = Malloc(double, _prob.l);
	svm_node **node = Malloc(svm_node*, _prob.l);

	for (size_t i = 0; i < posVector.size(); i++) {
		node[train_num_count] = getSVM_Node(posVector.at(i));
		_prob.y[train_num_count] = 1;
		++train_num_count;
	}

	for (size_t i = 0; i < negVector.size(); i++) {
		node[train_num_count] = getSVM_Node(negVector.at(i));
		_prob.y[train_num_count] = 2;
		++train_num_count;
	}
	_prob.x = node;
	svm_model *model = svm_train(&_prob, &_param);
	svm_save_model("model.txt", model);

}

void FaceRecognition::recognition() {

	// Clear Output Result Vector
	targetDirString.clear();
	double count = 1;
	double correct_num = 0;
	double total_num = 0;

	svm_model *model = svm_load_model("Resources/model.txt");
	if (model == NULL) {
		QMessageBox msg;
		msg.setText("Model can't find");
		msg.exec();
		return;
	}

	// Search Detect_Data Directory
	QDirIterator main_dir("Detect_Data");
	while (main_dir.hasNext()) {
		main_dir.next();
		if (main_dir.fileName() == "." || main_dir.fileName() == "..") {
			continue;
		}
		char assess_filename[50];
		memset(assess_filename, '\0', 50);
		sprintf(assess_filename, "%s/%s", main_dir.filePath().toUtf8().toStdString().c_str(), "Assessment_Result.txt");
		fstream read_stream(assess_filename);
		string readString;
		while (getline(read_stream, readString)) {
			istringstream isstream(readString);
			string file_Location;
			int label;
			isstream >> file_Location >> label;
			Mat image = imread("Detect_Data/" + file_Location, 0);
			cout << file_Location << endl;
			if (image.empty()) {
				cout << "Image read fail" << endl;
				continue;
			}

			if (label == count) {
				++total_num;
				double retval = svm_predict(model, getSVM_Node(image));
				if (retval == 1) {
					++correct_num;
				}
			}
			else {
				if (correct_num >= 3) {
					targetDirString.push_back(main_dir.fileName().toUtf8().toStdString());
				}
				correct_num = 0;
				total_num = 0;
				count++;
				return;
			}

		}
	}
}

void FaceRecognition::setTrainImage(vector<Mat> pTrainImage)
{
	trainImage = pTrainImage;
}

// Function will output two value: svm_node* or nullptr
Mat FaceRecognition::getFeatureVector(Mat face_image) {

	int feature_total_num = 3 * 7 * 16 * 59;
	int cell_size = 36;
	int feature_index = 0;
	Mat featureVector(1, feature_total_num, CV_32FC1, 0);

	if (face_image.channels() == 3) {
		cvtColor(face_image, face_image, CV_RGB2GRAY);
	}

	//svm_node *node_space = Malloc(svm_node, feature_total_num);
	faces.clear();
	faces = faceDetection(face_image);
	if (faces.empty()) {
		cout << "Face Detection 1 Error!!!" << endl;
		for (int i = 0; i < feature_total_num; i++) {
			featureVector.at<float>(0, i) = 0;
		}
		return featureVector;
	}

	if (faces.at(0).x - 20 < 0 ||
		faces.at(0).y - 20 < 0 ||
		(faces.at(0).x + faces.at(0).width) + 40 > face_image.cols ||
		(faces.at(0).y + faces.at(0).height) + 40 > face_image.rows) {
		cout << "Out of bound" << endl;
		for (int i = 0; i < feature_total_num; i++) {
			featureVector.at<float>(0, i) = 0;
		}
		return featureVector;
	}
	face_image = face_image(Rect(faces.at(0).x - 20, faces.at(0).y - 20, faces.at(0).width + 40, faces.at(0).height + 40));
	resize(face_image, face_image, Size(200, 200));

	faces.clear();
	faces = faceDetection(face_image);
	if (faces.size() == 0) {
		cout << "Face Detection 2 Error!!!" << endl;
		for (int i = 0; i < feature_total_num; i++) {
			featureVector.at<float>(0, i) = 0;
		}
		return featureVector;
	}

	double *landmarks = landmarkDetection(face_image, faces.at(0));
	if (*landmarks < 0) {
		for (int i = 0; i < feature_total_num; i++) {
			featureVector.at<float>(0, i) = 0;
		}
		return featureVector;
	}
	else {
		face_image = rotateImage(face_image, getSkewness(landmarks));
		landmarks = landmarkRotation(landmarks, -getSkewness(landmarks), face_image.cols / 2, face_image.rows / 2);
		// scale image
		for (size_t scale_num = 0; scale_num < 3; scale_num++) {
			Mat scaleImage;
			resize(face_image, scaleImage, Size(), 1 - 0.2*scale_num, 1 - 0.2*scale_num, 1);
			// can't find faces and landmarks
			for (size_t landmark_element = 2; landmark_element < 15; landmark_element += 2) {
				// crop image is out of bound
				if (isImgOutofBound(static_cast<int>(landmarks[landmark_element] * (1 - 0.2*scale_num)),
					static_cast<int>(landmarks[landmark_element + 1] * (1 - 0.2*scale_num)), cell_size, scaleImage.cols, scaleImage.rows)) {
					for (int histogram_num = 0; histogram_num < 16 * 59; histogram_num++) {
						featureVector.at<float>(0, feature_index) = 0;
						//node_space[feature_index].index = feature_index;
						//node_space[feature_index].value = 0;
						++feature_index;
					}
					continue;
				}
				else {
					Rect cropRect = getCropImageBound(landmarks[landmark_element] * (1 - 0.2*scale_num),
						landmarks[landmark_element + 1] * (1 - 0.2*scale_num), cell_size);
					Mat crop_Img = scaleImage(cropRect);
					for (size_t cell_y = 0; cell_y < cell_size; cell_y += 9) {
						for (size_t cell_x = 0; cell_x < cell_size; cell_x += 9) {
							Mat lbpFeatureVector;
							Rect cellRect(cell_x, cell_y, 9, 9);
							Mat crop_cell = crop_Img(cellRect);
							Mat lbpImage = lbpFeature(crop_cell);
							//histogram(lbpImage, lbpFeatureVector, 256);
							uni_histogram(lbpImage, lbpFeatureVector);
							for (int histogram_num = 0; histogram_num < 59; histogram_num++) {
								featureVector.at<float>(0, feature_index) = lbpFeatureVector.at<int>(0, histogram_num);
								//node_space[feature_index].index = feature_index;
								//node_space[feature_index].value = lbpFeatureVector.at<int>(0, histogram_num);  // (Scale)
								++feature_index;
							}
						}
					}
					crop_Img.release();
				}
			}
		}
		/*
		double retval = svm_predict(model, node_space);
		if (retval == 1) {
			correct_num++;
		}
		*/
	}
	return featureVector;
}

inline svm_node * FaceRecognition::getSVM_Node(Mat face_image) {
	int feature_total_num = 3 * 7 * 16 * 59 + 1;
	int cell_size = 36;
	int feature_index = 0;
	svm_node *node_space = Malloc(svm_node, feature_total_num);

	if (face_image.channels() == 3) {
		cvtColor(face_image, face_image, CV_RGB2GRAY);
	}

	//svm_node *node_space = Malloc(svm_node, feature_total_num);
	faces.clear();
	faces = faceDetection(face_image);
	if (faces.empty()) {
		cout << "Face Detection 1 Error!!!" << endl;
		for (int i = 0; i < feature_total_num - 1; i++) {
			node_space[i].index = i;
			node_space[i].value = 0;
		}
		node_space[feature_total_num - 1].index = -1;
		return node_space;
	}

	if (faces.at(0).x - 20 < 0 ||
		faces.at(0).y - 20 < 0 ||
		(faces.at(0).x + faces.at(0).width) + 40 > face_image.cols ||
		(faces.at(0).y + faces.at(0).height) + 40 > face_image.rows) {
		cout << "Out of bound" << endl;
		for (int i = 0; i < feature_total_num - 1; i++) {
			node_space[i].index = i;
			node_space[i].value = 0;
		}
		node_space[feature_total_num - 1].index = -1;
		return node_space;
	}

	face_image = face_image(Rect(faces.at(0).x - 20, faces.at(0).y - 20, faces.at(0).width + 40, faces.at(0).height + 40));
	resize(face_image, face_image, Size(200, 200));

	faces.clear();
	faces = faceDetection(face_image);
	if (faces.size() == 0) {
		cout << "Face Detection 2 Error!!!" << endl;
		for (int i = 0; i < feature_total_num; i++) {
			node_space[i].index = i;
			node_space[i].value = 0;
		}
		node_space[feature_total_num - 1].index = -1;
		return node_space;
	}

	double *landmarks = landmarkDetection(face_image, faces.at(0));
	if (*landmarks < 0) {
		for (int i = 0; i < feature_total_num; i++) {
			node_space[i].index = i;
			node_space[i].value = 0;
		}
		node_space[feature_total_num - 1].index = -1;
		return node_space;
	}
	else {
		face_image = rotateImage(face_image, getSkewness(landmarks));
		landmarks = landmarkRotation(landmarks, -getSkewness(landmarks), face_image.cols / 2, face_image.rows / 2);
		// scale image
		for (size_t scale_num = 0; scale_num < 3; scale_num++) {
			Mat scaleImage;
			resize(face_image, scaleImage, Size(), 1 - 0.2*scale_num, 1 - 0.2*scale_num, 1);
			// can't find faces and landmarks
			for (size_t landmark_element = 2; landmark_element < 15; landmark_element += 2) {
				// crop image is out of bound
				if (isImgOutofBound(static_cast<int>(landmarks[landmark_element] * (1 - 0.2*scale_num)),
					static_cast<int>(landmarks[landmark_element + 1] * (1 - 0.2*scale_num)), cell_size, scaleImage.cols, scaleImage.rows)) {
					for (int histogram_num = 0; histogram_num < 16 * 59; histogram_num++) {
						node_space[feature_index].index = feature_index;
						node_space[feature_index].value = 0;
						++feature_index;
					}
					continue;
				}
				else {
					Rect cropRect = getCropImageBound(landmarks[landmark_element] * (1 - 0.2*scale_num),
						landmarks[landmark_element + 1] * (1 - 0.2*scale_num), cell_size);
					Mat crop_Img = scaleImage(cropRect);
					for (size_t cell_y = 0; cell_y < cell_size; cell_y += 9) {
						for (size_t cell_x = 0; cell_x < cell_size; cell_x += 9) {
							Mat lbpFeatureVector;
							Rect cellRect(cell_x, cell_y, 9, 9);
							Mat crop_cell = crop_Img(cellRect);
							Mat lbpImage = lbpFeature(crop_cell);
							//histogram(lbpImage, lbpFeatureVector, 256);
							uni_histogram(lbpImage, lbpFeatureVector);
							for (int histogram_num = 0; histogram_num < 59; histogram_num++) {
								//featureVector.at<float>(0, feature_index) = lbpFeatureVector.at<int>(0, histogram_num);
								node_space[feature_index].index = feature_index;
								node_space[feature_index].value = lbpFeatureVector.at<int>(0, histogram_num);  // (Scale)
								++feature_index;
							}
						}
					}
					crop_Img.release();
				}
			}
		}
	}
	return node_space;
}

