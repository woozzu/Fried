#include "MachineLearner.h"

namespace munchkin
{
	void MachineLearner::Load(const char* filename)
	{
		m_svm.load(filename);
	}

	void MachineLearner::Save(const char* filename)
	{
		m_svm.save(filename);
	}

	void MachineLearner::Train(const Mat& trainData, const Mat& responses)
	{
		CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
		CvSVMParams params(CvSVM::C_SVC, CvSVM::RBF, 0, 0.03125, 0, 128, 0, 0, NULL, criteria);
		m_svm.train_auto(trainData, responses, Mat(), Mat(), params, 12);
		//m_svm.train(trainData, responses, Mat(), Mat(), params);
	}

	float MachineLearner::Predict(const Mat& sample)
	{
		return m_svm.predict(sample);
	}
}