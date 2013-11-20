#include "Common.h"

namespace munchkin
{
	class ImageSegmenter
	{
	public:
		static void Segment(const char* filename, Mat* segments, int row, int col);
	};
}
