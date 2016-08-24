#include "FaceTracking.h"

FaceTracking::FaceTracking()
{
	face_cascade_name = "Resources/haarcascade_frontalface_alt.xml";
	dirName = "Crop_Image";
}

FaceTracking::~FaceTracking()
{

}

vector<Rect> FaceTracking::faceDetection(Mat frame)
{
	CascadeClassifier face_cascade;
	vector<Rect> detect_faces;
	if (!face_cascade.load(face_cascade_name)) {
		printf("Error Loading");
	}
	face_cascade.detectMultiScale(frame, detect_faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cvSize(90, 90));
	return detect_faces;
}

void FaceTracking::createDirectory(VideoCapture &capture, FileStorage &fs, Mat &last_gray, vector<Rect> &faces, vector<TLD_Info> &vtld_info, int &dirCounter, double &frame_num, string dirName)
{
	Mat frame;
	while (faces.size() == 0) {
		capture >> frame;
		//frame_num += 3;
		//capture.set(CV_CAP_PROP_POS_FRAMES, frame_num);
		cvtColor(frame, last_gray, CV_RGB2GRAY);
		faces = faceDetection(last_gray);
	}

	// create directory && initialize TLD && set up parameters
	char dirPath[30];
	char filePath[30];
	memset(dirPath, '\0', 30);
	memset(filePath, '\0', 30);

	for (size_t i = 0; i < faces.size(); i++) {
		// increase face size to avoid detection fail
		faces[i].x = faces[i].x - 70;
		faces[i].y = faces[i].y - 70;
		faces[i].width = faces[i].width + 140;
		faces[i].height = faces[i].height + 140;
		if (faces[i].x < 0 || faces[i].y < 0 ||
			(faces[i].x + faces[i].width) > frame.cols ||
			(faces[i].y + faces[i].height) > frame.rows) {
			faces.erase(faces.begin() + i);
			continue;
		}

		TLD_Info tempTLD_Info;
		TLD *tempTLD = new TLD();
		tempTLD->read(fs.getFirstTopLevelNode());
		tempTLD->init(last_gray, faces[i]);
		// set TLD information: TLD dirCounter fileCounter remainFrame
		setTLD_Info(tempTLD_Info, tempTLD, dirCounter, 2, 4);
		sprintf(dirPath, "./%s/%03d/", dirName.c_str(), dirCounter);
		mkdir(dirPath);

		Mat face_image = frame(faces[i]);
		sprintf(filePath, "./%s/%03d/%03d.jpg", dirName.c_str(), dirCounter, 1);
		imwrite(filePath, face_image);

		++dirCounter;
		vtld_info.push_back(tempTLD_Info);
		// vTLD.push_back(tld);
	}
}

void FaceTracking::setDirName(string dir_name)
{
	dirName = dir_name;
}

void FaceTracking::setMaxValue(double pMaxValue)
{
	max_value = pMaxValue;
}

void FaceTracking::setVideoName(string pVideoName)
{
	videoName = pVideoName;
}

void FaceTracking::setTLD_Info(TLD_Info &tld_info, TLD *tld, int dirCounter, int fileCounter, int remainFrame)
{
	tld_info.tld = tld;
	tld_info.dirCounter = dirCounter;
	tld_info.fileCounter = fileCounter;
	tld_info.remainFrame = remainFrame;
}

void FaceTracking::freeTLD_Info(vector<TLD_Info> &vtld_info)
{
	for (size_t i = 0; i < vtld_info.size(); i++) {
		delete vtld_info[i].tld;
	}
}

void FaceTracking::run()
{
	// parameters setting
	VideoCapture capture;
	capture.open(videoName);
	if (!capture.isOpened()) {
		return;
	}

	// Initialize variable
	vector<TLD_Info> vtld_info;
	FileStorage fs;
	fs.open("Resources/parameters.yml", FileStorage::READ);
	int skip_msecs = 0;
	int dirCounter = 1;
	double frame_num = 0;

	Mat frame;
	Mat last_gray;

	char mainDir[30];
	memset(mainDir, '\0', 30);
	sprintf(mainDir, "./%s/", dirName.c_str());
	mkdir(mainDir);
	createDirectory(capture, fs, last_gray, faces, vtld_info, dirCounter, frame_num, dirName);

	Mat current_gray;
	BoundingBox pbox;
	vector<Point2f> pts1;
	vector<Point2f> pts2;
	bool status = true;
	bool tl = true;

	int remainFrame = 5;

	while (capture.read(frame)) {
		//frame_num += 3;
		//capture.set(CV_CAP_PROP_POS_FRAMES, frame_num);
		cvtColor(frame, current_gray, CV_RGB2GRAY);

		if (remainFrame > 0 && vtld_info.size() > 0) {
			// correct_num caculate how many faces should we track
			int correct_num = 0;
			for (size_t i = 0; i < vtld_info.size(); i++) {
				char filePath[30];
				memset(filePath, '\0', 30);
				TLD_Info tempTLD_Info = vtld_info[i];
				tempTLD_Info.tld->processFrame(last_gray, current_gray, pts1, pts2,
					pbox, status, tl);

				// success tracking
				if (status) {
					if (pbox.x < 0 || pbox.y < 0 || (pbox.x + pbox.width) > frame.cols ||
						(pbox.y + pbox.height) > frame.rows) {
						delete vtld_info[i].tld;
						vtld_info.erase(vtld_info.begin() + i);
						continue;
					}
					Mat face_image = frame(pbox);
					// Test Code
					if (tempTLD_Info.fileCounter % 5 == 0) {
						//vector<Rect>().swap(faces);
						faces = faceDetection(face_image);
						if (faces.size() == 0 && faces.size() != vtld_info.size()) {
							correct_num = -1; // if (correct_num != vtld_info.size()), stop
										     // tracking
							remainFrame = -1;
							pts1.clear();
							pts2.clear();
							break;
						}
					}
					sprintf(filePath, "./%s/%03d/%03d.jpg", dirName.c_str(),
						tempTLD_Info.dirCounter, tempTLD_Info.fileCounter);
					imwrite(filePath, face_image);
					++tempTLD_Info.fileCounter;
					vtld_info[i] = tempTLD_Info;

					// show video tracking state
					drawPoints(frame, pts1);
					drawPoints(frame, pts2, Scalar(0, 255, 0));
					drawBox(frame, pbox);
					imshow("Tracking", frame);
					if (waitKey(10) >= 0) {
						break;
					}
					++correct_num;
				}
				else {
					--correct_num;
				}
			}
			pts1.clear();
			pts2.clear();

			// if program can't detect
			if (correct_num == vtld_info.size()) {
				remainFrame = 5;
			}
			else {
				--remainFrame;
			}
			swap(last_gray, current_gray);
		}
		else {
			std::cout << "Restart 1" << std::endl;
			// erase vector
			vector<Point2f>().swap(pts1);
			vector<Point2f>().swap(pts2);
			freeTLD_Info(vtld_info);
			std::cout << "Restart 2" << std::endl;
			vector<TLD_Info>().swap(vtld_info);
			//vector<Rect>().swap(faces);
			std::cout << "Restart 3" << std::endl;
			createDirectory(capture, fs, last_gray, faces, vtld_info, dirCounter,
				frame_num, dirName);
			std::cout << "Restart 4" << std::endl;
			remainFrame = 5;
			std::cout << "Restart 5" << std::endl;
		}
		// imshow("Test", frame);
		// waitKey(1);
	}
}
