#pragma once
#include "type.h"
#include <vector>
#include <opencv2/opencv.hpp>

//规格化	impl in filter.cpp
extern cv::Mat normalize(const cv::Mat &img);

//方向图 impl in filter.cpp
extern cv::Mat directionalOrientation(const cv::Mat &nor);

//方向图块化 impl in filter.cpp
extern void toBlockOrientation(cv::Mat &ori);

//方向滤波 impl in filter.cpp
extern cv::Mat directionalFilter(const cv::Mat &nor, const cv::Mat &ori);

//二值化去燥 impl in filter.cpp
extern void thresholdImageDenosing(cv::Mat &img);

//细化 impl in thinning.cpp
extern void thinningImage(cv::Mat &img, int maxIter = 16);

//细化优化 impl in thinning.cpp
extern void optimizeThinnedImage(cv::Mat &img);

//删除孤立点和毛刺、短线 impl in thinning.cpp
extern void deletePointAndShortLine(cv::Mat &img);

//获取特征点集 impl in trait.cpp
extern std::vector<TraitPoint> getTraitPoints(const cv::Mat &img, const cv::Mat &ori);

//提取特征点 impl in trait.cpp
extern std::vector<TraitPoint> extractTraitPoints(const cv::Mat &img, const cv::Mat &ori);

//显示特征点 impl in trait.cpp
extern cv::Mat showTraitPoints(const cv::Mat &img);
extern cv::Mat showTraitPoints(const cv::Mat &img, const cv::Mat &ori);

//获取局部特征向量
extern std::vector<LocalTrait> getLocalTrait(const cv::Mat img, const std::vector<TraitPoint> va);

//匹配 impl in match.cpp
extern std::vector<int> match(cv::Mat aimg, cv::Mat aori, cv::Mat bimg, cv::Mat bori);
extern std::vector<int> match(
	std::vector<TraitPoint> va, std::vector<LocalTrait> avec,
	std::vector<TraitPoint> vb, std::vector<LocalTrait> bvec);

extern std::vector<int> recognize(
	std::ifstream &in, 
	std::vector<TraitPoint> va, 
	std::vector<LocalTrait> avec);

//二值化 impl in core.cpp
extern void thresholdImage(cv::Mat &nor);

//预处理 impl in core.cpp
extern void preprocess(cv::Mat &nor, cv::Mat &ori);

extern bool writeTraits(std::ofstream &out, std::vector<TraitPoint> tp, std::vector<LocalTrait> lt);
