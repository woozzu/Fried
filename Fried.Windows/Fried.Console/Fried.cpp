#include <iostream>
#include <fstream>
#include "../../Fried.Core/ImageSegmenter.h"
#include "../../Fried.Core/FeatureExtractor.h"
#include "../../Fried.Core/MachineLearner.h"

#include <ctype.h>
#include <io.h>

using namespace cv;
using namespace std;
using namespace munchkin;

void trainFromDb(const char* dbPath);
void train(const char* path, Mat& feature, vector<int>& responses, int category = 0);
void testFromDb(const char* dbPath);
void test(const char* path, MachineLearner& ml, ofstream& report, int category = 0);

int main(int argc, char** argv)
{
	try
	{
		testFromDb("..\\..\\Fried.Db");
	} catch (Exception e1) {
		cout << e1.msg << endl;
	} catch (exception e2) {
		cout << e2.what() << endl;
	}
	return 0;
}

void trainFromDb(const char* dbPath)
{
	Mat feature;
	vector<int> responses;

	train(dbPath, feature, responses);

	if (feature.rows == 0)
		throw exception("There is no train data.");
	MachineLearner ml;
	ml.Train(feature, Mat(responses));
	ml.Save("..\\fried_segements_svm.dat");
}

void train(const char* path, Mat& feature, vector<int>& responses, int category)
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
			if (j > 60)
				continue;

			Mat f;
			Mat image = imread(filename.str().c_str());
			FeatureExtractor::GetFeatures_linHSVhist_hog(image, f);
			feature.push_back(f);
			responses.push_back(category);
			j++;
		} else if (strcmp(fd.name, "..") != 0 && strcmp(fd.name, ".") != 0) {
			train(filename.str().c_str(), feature, responses, i++);
		}
	} while (_findnext(h, &fd) != -1);

	_findclose(h);
}

void testFromDb(const char* dbPath)
{
	MachineLearner ml;
	ml.Load("..\\fried_segements_svm.dat");
	ofstream report("..\\fried_test_report.txt");
	test(dbPath, ml, report);
}

void test(const char* path, MachineLearner& ml, ofstream& report, int category)
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
			FeatureExtractor::GetFeatures_linHSVhist_hog(image, f);
			int p = (int)ml.Predict(f);
			if (p == category)
				j++;
			report << "(" << p << ")" << fd.name << endl;
		} else if (strcmp(fd.name, "..") != 0 && strcmp(fd.name, ".") != 0) {
			report << "[" << filename.str() << "]" << endl;
			test(filename.str().c_str(), ml, report, i++);
		}
	} while (_findnext(h, &fd) != -1);

	if (inDir)
		report << "Prediction Rate: " << (float)j / k << endl;
	_findclose(h);
}

//void SegmentFunc(void *pParam)
//{
//	FindFile(_T("d:\\development\\Munchkin\\Fried.All\\Fried.Db"));
//}
//
//void FindFile(string dir)
//{
//	CFileFind fileFind;
//	BOOL found = fileFind.FindFile(dir + _T("\\*.*"));
//	while (found) {
//		found = fileFind.FindNextFileW();
//		if (fileFind.IsDots())
//			continue;
//		if (fileFind.IsDirectory())
//			FindFile(fileFind.GetFilePath());
//
//		CT2CA pszConvertedAnsiString(fileFind.GetFilePath());
//		string filePath(pszConvertedAnsiString);
//
//		Mat **segments = new Mat*[10];
//		for (int i = 0; i < 10; i++)
//			segments[i] = new Mat[10];
//		ImageSegmenter::Segment(filePath, segments, 10, 10);
//
//		vector<int> params;
//		params.push_back(CV_IMWRITE_JPEG_QUALITY);
//		params.push_back(95);
//		for (int i = 0; i < 10; i++) {
//			for (int j = 0; j < 10; j++) {
//				ostringstream oss;
//				oss << filePath << ".segment_" << i << "_" << j << ".jpg";
//				imwrite(oss.str(), segments[i][j], params);
//			}
//		}
//	}
//}