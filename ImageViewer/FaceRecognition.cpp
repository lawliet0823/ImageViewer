#include "FaceRecognition.h"

FaceRecognition::FaceRecognition()
{
	face_cascade_name = "haarcascade_frontalface_alt_tree.xml";
	landmarkModel = flandmark_init("flandmark_model.dat");
	feature_total_num = 3 * 7 * 16 * 59 + 1;
	cell_size = 36;

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

Mat FaceRecognition::lbpFeature(Mat image)
{
	Mat dst;
	GaussianBlur(image, dst, Size(5, 5), 5, 3, BORDER_CONSTANT);
	Mat lbpImage = ELBP(dst, 2, 8);
	return lbpImage;
}

inline bool FaceRecognition::isImgOutofBound(int x, int y, int crop_size, int img_width, int img_height)
{
	if (x - crop_size / 2 < 0 || x + crop_size / 2 > img_width || y - crop_size / 2 < 0 || y + crop_size / 2 > img_height)
		return true;
	else
		return false;
}

void FaceRecognition::train_model(int ptrain_num)
{
	_prob.l = ptrain_num;
	_prob.y = Malloc(double, _prob.l);
	svm_node **node = Malloc(svm_node*, _prob.l);
	int train_num_count = 0;
	for (size_t i = 0; i < trainImage.size(); i++) {
		Mat face_image = trainImage.at(i);
		cvtColor(face_image, face_image, CV_RGB2GRAY);
		equalizeHist(face_image, face_image);
		int feature_index = 0;
		svm_node *node_space = Malloc(svm_node, feature_total_num);
		// Face Detection
		vector<Rect> faces = faceDetection(face_image);
		if (faces.size() == 0) {
			for (int i = 0; i < feature_total_num - 1; i++) {
				node_space[i].index = i;
				node_space[i].value = 0;
			}
			node_space[feature_total_num - 1].index = -1;
			node[train_num_count] = node_space;
			_prob.y[train_num_count] = 1;
			++train_num_count;
			continue;
		}

		double *landmarks = landmarkDetection(face_image, faces[0]);
		if (*landmarks < 0) {
			for (int i = 0; i < feature_total_num - 1; i++) {
				node_space[i].index = i;
				node_space[i].value = 0;
			}
			node_space[feature_total_num - 1].index = -1;
			node[train_num_count] = node_space;
			_prob.y[train_num_count] = 1;
			++train_num_count;
			continue;
		}
		else {
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
					//cout << landmarks[landmark_element] - 20 << "	" << landmarks[landmark_element+1]-20 << endl;
					else {
						Rect cropRect = getCropImageBound(landmarks[landmark_element] * (1 - 0.2*scale_num),
							landmarks[landmark_element + 1] * (1 - 0.2*scale_num), cell_size);
						Mat crop_Img = scaleImage(cropRect);
						//imshow("", scaleImage);
						//waitKey(0);

						Mat mat_pca(16, 59, CV_32FC1);
						int sample_count = 0;
						for (size_t cell_y = 0; cell_y < cell_size; cell_y += 9) {
							for (size_t cell_x = 0; cell_x < cell_size; cell_x += 9) {
								Mat lbpFeatureVector;	// Feature Vector
								Rect cellRect(cell_x, cell_y, 9, 9);
								Mat crop_cell = crop_Img(cellRect);
								Mat lbpImage = lbpFeature(crop_cell);
								//histogram(lbpImage, lbpFeatureVector, 256);
								uni_histogram(lbpImage, lbpFeatureVector);
								for (int histogram_num = 0; histogram_num < 59; histogram_num++) {
									//mat_pca.at<int>(sample_count, histogram_num) = lbpFeatureVector.at<int>(0, histogram_num);
									//cout << lbpFeatureVector.at<int>(0, histogram_num) << "		";

									node_space[feature_index].index = feature_index;
									node_space[feature_index].value = lbpFeatureVector.at<int>(0, histogram_num);  // (Scale)
									++feature_index;

									//cout << histogram_num << "	" << lbpFeatureVector.at<int>(0, histogram_num) << endl;
								}
								//cout << endl;
								//lbpFeatureVector.release();
								//crop_cell.release();
								//lbpImage.release();
							}
						}
						/*
						PCA pca(mat_pca, Mat(), CV_PCA_DATA_AS_ROW);
						cout << pca.eigenvectors.size().width << "		" << pca.eigenvectors.size().height << endl;
						for (size_t i = 0; i < pca.eigenvectors.size().width; i++) {
						cout << pca.eigenvectors.at<float>(13, i) << "	";
						}
						cout << endl;
						*/
						//crop_Img.release();
					}
				}
			}
			node_space[feature_total_num - 1].index = -1;
			node[train_num_count] = node_space;
			_prob.y[train_num_count] = 1;
			++train_num_count;
		}
	}

	/*
	for (int i = 0; i < feature_total_num - 1; i++) {
		node_space[i].index = i;
		node_space[i].value = 0;
	}
	node_space[feature_total_num - 1].index = -1;
	node[train_num_count] = node_space;
	_prob.y[train_num_count] = 1;
	++train_num_count;
	continue;
	*/

	_prob.x = node;
	svm_model *model = svm_train(&_prob, &_param);
	svm_save_model("model.txt", model);
}
