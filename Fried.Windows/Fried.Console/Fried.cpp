#include <iostream>
#include <fstream>
#include "../../Fried.Core/ImageSegmenter.h"
#include "../../Fried.Core/FeatureExtractor.h"
#include "../../Fried.Core/MachineLearner.h"

#include <stdlib.h>
#include <ctype.h>
#include <io.h>

using namespace cv;
using namespace std;
using namespace munchkin;

void printError();
void trainFromDb(const char* dbPath, int count, const char* outfilename, bool hsv, int hbins, int sbins, int vbins);
void train(const char* path, int count, bool hsv, int hbins, int sbins, int vbins
		   , Mat& feature, vector<int>& responses, int category = 0);
void testFromDb(const char* trainedFilename, const char* dbPath, const char* outfilename, bool hsv, int hbins, int sbins, int vbins);
void test(const char* path, bool hsv, int hbins, int sbins, int vbins, MachineLearner& ml, ofstream& report, int category = 0);
void segment(const char* path);

int main(int argc, char** argv)
{
	Mat gray, sobel, canny;
	Mat image = imread("..\\foliage.jpg");
	cvtColor(image, gray, CV_RGB2GRAY);

	namedWindow("original", CV_WINDOW_NORMAL);
	namedWindow("canny", CV_WINDOW_NORMAL);

	imshow("original", image);

	Canny(gray, canny, 100, 300);
	imshow("canny", canny);

	waitKey();
	destroyAllWindows();

	/*if (argc == 1) {
		printError();
	} else if (strcmp(argv[1], "train") == 0) {
		if (argc < 5) {
			printError();
		} else {
			char* dir = argv[2];
			int count = atoi(argv[3]);
			char* outfilename = argv[4];
			bool hsv = false;
			int hbins = 0, sbins = 0, vbins = 0;
			if (argc >= 6 && strcmp(argv[5], "hsv") == 0) {
				hsv = true;
				hbins = atoi(argv[6]);
				sbins = atoi(argv[7]);
				vbins = atoi(argv[8]);
			}

			try
			{
				trainFromDb(dir, count, outfilename, hsv, hbins, sbins, vbins);
			} catch (Exception e1) {
				cout << e1.msg << endl;
			} catch (exception e2) {
				cout << e2.what() << endl;
			}
		}
	} else if (strcmp(argv[1], "test") == 0) {
		if (argc < 5) {
			printError();
		} else {
			bool hsv = false;
			int hbins = 0, sbins = 0, vbins = 0;
			if (argc >= 6 && strcmp(argv[5], "hsv") == 0) {
				hsv = true;
				hbins = atoi(argv[6]);
				sbins = atoi(argv[7]);
				vbins = atoi(argv[8]);
			}

			try
			{
				testFromDb(argv[2], argv[3], argv[4], hsv, hbins, sbins, vbins);
			} catch (Exception e1) {
				cout << e1.msg << endl;
			} catch (exception e2) {
				cout << e2.what() << endl;
			}
		}
	} else if (strcmp(argv[1], "segment") == 0) {
		if (argc < 3) {
			printError();
		} else {
			segment(argv[2]);
		}
	}*/

	return 0;
}

void printError()
{
	cout << "Bad arguments." << endl;
	cout << "train w/o hsv: train [image_db_dir] [image_count_by_one_category] [out_trained_filename]" << endl;
	cout << "train w hsv: train [image_db_dir] [image_count_by_one_category] [out_trained_filename] hsv [hbins] [sbins] [vbins]" << endl;
	cout << "test: test [trained_filename] [image_db_dir] [report_filename]" << endl;
	cout << "segment: segment [image_dir]" << endl;
}

void trainFromDb(const char* dbPath, int count, const char* outfilename, bool hsv, int hbins, int sbins, int vbins)
{
	Mat feature;
	vector<int> responses;

	train(dbPath, count, hsv, hbins, sbins, vbins, feature, responses);

	if (feature.rows == 0)
		throw exception("There is no train data.");
	MachineLearner ml;
	ml.Train(feature, Mat(responses));
	ml.Save(outfilename);
}

void train(const char* path, int count, bool hsv, int hbins, int sbins, int vbins
		   , Mat& feature, vector<int>& responses, int category)
{
	_finddata_t fd;
	stringstream filePath;
	filePath << path << "\\*.*";
	intptr_t h = _findfirst(filePath.str().c_str(), &fd);
	if (h == -1)
		return;

	int i = 1;
	int j = 1;
	do {
		stringstream filename;
		filename << path << "\\" << fd.name;

		if (fd.size > 0) {
			if (j > count)
				continue;

			Mat f;
			Mat image = imread(filename.str().c_str());
			if (hsv)
				FeatureExtractor::GetFeatures_linHSVhist_hog(image, f, hbins, sbins, vbins);
			else
				FeatureExtractor::GetHOGHist(image, f);
			feature.push_back(f);
			responses.push_back(category);
			j++;
		} else if (strcmp(fd.name, "..") != 0 && strcmp(fd.name, ".") != 0) {
			train(filename.str().c_str(), count, hsv, hbins, sbins, vbins, feature, responses, i++);
		}
	} while (_findnext(h, &fd) != -1);

	_findclose(h);
}

void testFromDb(const char* trainedFilename, const char* dbPath, const char* outfilename, bool hsv, int hbins, int sbins, int vbins)
{
	MachineLearner ml;
	ml.Load(trainedFilename);
	ofstream report(outfilename);
	test(dbPath, hsv, hbins, sbins, vbins, ml, report);
}

void test(const char* path, bool hsv, int hbins, int sbins, int vbins, MachineLearner& ml, ofstream& report, int category)
{
	_finddata_t fd;
	stringstream filePath;
	filePath << path << "\\*.*";
	intptr_t h = _findfirst(filePath.str().c_str(), &fd);
	if (h == -1)
		return;

	int i = 1;
	int j = 0;
	int k = 0;
	bool inDir = false;
	do {
		stringstream filename;
		filename << path << "\\" << fd.name;

		if (fd.size > 0) {
			inDir = true;
			k++;

			Mat f;
			Mat image = imread(filename.str().c_str());
			if (hsv)
				FeatureExtractor::GetFeatures_linHSVhist_hog(image, f, hbins, sbins, vbins);
			else
				FeatureExtractor::GetHOGHist(image, f);
			int p = (int)ml.Predict(f);
			if (p == category)
				j++;
			report << "(" << p << ")" << fd.name << endl;
		} else if (strcmp(fd.name, "..") != 0 && strcmp(fd.name, ".") != 0) {
			report << "[" << filename.str() << "]" << endl;
			test(filename.str().c_str(), hsv, hbins, sbins, vbins, ml, report, i++);
		}
	} while (_findnext(h, &fd) != -1);

	if (inDir)
		report << "Prediction Rate: " << (float)j / k << endl;
	_findclose(h);
}

void segment(const char* path)
{
	_finddata_t fd;
	stringstream filePath;
	filePath << path << "\\*.*";
	intptr_t h = _findfirst(filePath.str().c_str(), &fd);
	if (h == -1)
		return;

	do {
		stringstream filename;
		filename << path << "\\" << fd.name;

		if (fd.size > 0) {
			Mat segments[100];
			ImageSegmenter::Segment(filename.str().c_str(), segments, 10, 10);
	
			vector<int> params;
			params.push_back(CV_IMWRITE_JPEG_QUALITY);
			params.push_back(95);
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					ostringstream oss;
					oss << filename.str() << ".segment_" << i << "_" << j << ".jpg";
					imwrite(oss.str(), segments[i * 10 + j], params);
				}
			}
		} else if (strcmp(fd.name, "..") != 0 && strcmp(fd.name, ".") != 0) {
			segment(filename.str().c_str());
		}
	} while (_findnext(h, &fd) != -1);

	_findclose(h);
}