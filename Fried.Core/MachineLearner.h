#include "Common.h"

namespace munchkin
{
	class MachineLearner
	{
	public:
		void Save(const char* filename);
		void Load(const char* filename);
		void Train(const Mat& trainData, const Mat& responses);
		float Predict(const Mat& sample);

	private:
		CvSVM m_svm;
	};
}