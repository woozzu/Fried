#include "Common.h"

namespace munchkin
{
	class FeatureExtractor
	{
	public:
		static void GetLinHSVHist(const Mat& image, Mat& hist, int hbins, int sbins, int vbins);
		static void GetHOGHist(const Mat& image, Mat& hist);

		static void GetFeatures_linHSVhist_hog(const Mat& image, Mat& feature);
	};
}