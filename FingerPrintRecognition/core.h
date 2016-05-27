#pragma once
#include "type.h"
#include <vector>
#include <opencv2/opencv.hpp>

//���	impl in filter.cpp
extern cv::Mat normalize(const cv::Mat &img);

//����ͼ impl in filter.cpp
extern cv::Mat directionalOrientation(const cv::Mat &nor);

//����ͼ�黯 impl in filter.cpp
extern void toBlockOrientation(cv::Mat &ori);

//�����˲� impl in filter.cpp
extern cv::Mat directionalFilter(const cv::Mat &nor, const cv::Mat &ori);

//��ֵ��ȥ�� impl in filter.cpp
extern void thresholdImageDenosing(cv::Mat &img);

//ϸ�� impl in thinning.cpp
extern void thinningImage(cv::Mat &img, int maxIter = 16);

//ϸ���Ż� impl in thinning.cpp
extern void optimizeThinnedImage(cv::Mat &img);

//ɾ���������ë�̡����� impl in thinning.cpp
extern void deletePointAndShortLine(cv::Mat &img);

//��ȡ�����㼯 impl in trait.cpp
extern std::vector<TraitPoint> getTraitPoints(const cv::Mat &img, const cv::Mat &ori);

//��ȡ������ impl in trait.cpp
extern std::vector<TraitPoint> extractTraitPoints(const cv::Mat &img, const cv::Mat &ori);

//��ʾ������ impl in trait.cpp
extern cv::Mat showTraitPoints(const cv::Mat &img);
extern cv::Mat showTraitPoints(const cv::Mat &img, const cv::Mat &ori);

//��ȡ�ֲ���������
extern std::vector<LocalTrait> getLocalTrait(const cv::Mat img, const std::vector<TraitPoint> va);

//ƥ�� impl in match.cpp
extern std::vector<int> match(cv::Mat aimg, cv::Mat aori, cv::Mat bimg, cv::Mat bori);
extern std::vector<int> match(
	std::vector<TraitPoint> va, std::vector<LocalTrait> avec,
	std::vector<TraitPoint> vb, std::vector<LocalTrait> bvec);

extern std::vector<int> recognize(
	std::ifstream &in, 
	std::vector<TraitPoint> va, 
	std::vector<LocalTrait> avec);

//��ֵ�� impl in core.cpp
extern void thresholdImage(cv::Mat &nor);

//Ԥ���� impl in core.cpp
extern void preprocess(cv::Mat &nor, cv::Mat &ori);

extern bool writeTraits(std::ofstream &out, std::vector<TraitPoint> tp, std::vector<LocalTrait> lt);
