#include "stdafx.h"
#include "core.h"

void preprocess(cv::Mat &nor, cv::Mat &ori) {
	 ori = directionalOrientation(nor);
	 toBlockOrientation(ori);
	 nor = directionalFilter(nor, ori);
	 cv::adaptiveThreshold(nor, nor, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, 1);
	 thresholdImageDenosing(nor);
	 thinningImage(nor);
	 optimizeThinnedImage(nor);
	 deletePointAndShortLine(nor);
}