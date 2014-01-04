#include "FeatureExtractor.h"

namespace munchkin
{
	void FeatureExtractor::GetLinHSVHist(const Mat& image, Mat& hist, int hbins, int sbins, int vbins)
	{
		MatND histogram, norm;
		Mat hsv;
		cvtColor(image, hsv, CV_RGB2HSV);

		float hranges[] = { 0, 180 };
		float sranges[] = { 0, 256 };
		float vranges[] = { 0, 256 };

		int channels[] = { 0, 1, 2 };
		int histSize[] = { hbins, sbins, vbins };
		const float* ranges[] = { hranges, sranges, vranges };
		calcHist(&hsv, 1, channels, Mat(), histogram, 3, histSize, ranges);

		vector<float> linHist;
		for (int i = 0; i < hbins; i++) {
			for (int j = 0; j < sbins; j++) {
				for (int k = 0; k < vbins; k++) {
					linHist.push_back(histogram.at<float>(i, j, k));
				}
			}
		}
		normalize(Mat(linHist), hist);
		transpose(hist, hist);
	}

	void FeatureExtractor::GetHOGHist(const Mat& image, Mat& hist)
	{
		Mat resized, resizedGray;
		vector<float> descriptors;
		resize(image, resized, Size(64, 128));
		cvtColor(resized, resizedGray, CV_RGB2GRAY);

		HOGDescriptor hog;
		hog.compute(resizedGray, descriptors);
		normalize(Mat(descriptors), hist);
		transpose(hist, hist);
	}

	void FeatureExtractor::GetFeatures_linHSVhist_hog(const Mat& image, Mat& feature)
	{
		GetFeatures_linHSVhist_hog(image, feature, 30, 26, 4);
	}

	void FeatureExtractor::GetFeatures_linHSVhist_hog(const Mat& image, Mat& feature, int hbins, int sbins, int vbins)
	{
		Mat hsvHist, hogHist;
		GetLinHSVHist(image, hsvHist, hbins, sbins, vbins);
		GetHOGHist(image, hogHist);
		hconcat(hsvHist, hogHist, feature);
	}
}