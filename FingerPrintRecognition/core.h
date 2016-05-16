#pragma once
#include "type.h"
#include <vector>
#include <opencv2/opencv.hpp>

extern cv::Mat normalize(const cv::Mat &img);

extern cv::Mat directionalOrientation(const cv::Mat &nor);

extern void toBlockOrientation(cv::Mat &ori);

extern cv::Mat directionalFilter(const cv::Mat &nor, const cv::Mat &ori);

extern void thresholdImageDenosing(cv::Mat &img);

extern void thinningImage(cv::Mat &img, int maxIter = 16);

extern void optimizeThinnedImage(cv::Mat &img);

extern void deletePointAndShortLine(cv::Mat &img);

extern std::vector<TraitPoint> getTraitPoints(const cv::Mat &img, const cv::Mat &ori);

extern std::vector<TraitPoint> extractTraitPoints(const cv::Mat &img, const cv::Mat &ori);

extern cv::Mat showTraitPoints(const cv::Mat &img, const cv::Mat &ori);

extern std::vector<int> match(cv::Mat aimg, cv::Mat aori, cv::Mat bimg, cv::Mat bori);

extern void preprocess(cv::Mat &nor, cv::Mat &ori);