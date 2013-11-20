#include "ImageSegmenter.h"

namespace munchkin
{
	void ImageSegmenter::Segment(const char* filename, Mat* segments, int row, int col)
	{
		Mat img = imread(string(filename));
		int width = img.size().width / row;
		int height = img.size().height / col;

		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				Rect rect(width * i, height * j, width, height);
				img(rect).copyTo(segments[i * row + j]);
			}
		}

		img.release();
	}
}
