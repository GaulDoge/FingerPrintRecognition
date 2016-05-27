#include "stdafx.h"
#include "type.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <opencv2/opencv.hpp>

using std::vector;

using cv::Mat;
using cv::Point;

//获取特征点图
static Mat getTraitImage(const Mat &img) {
	assert(img.type() == CV_8UC1);
	Mat rst(img.size(), CV_8UC1, cv::Scalar::all(0));//zero mark point not be a trait point
	for (int i = 1, h = img.rows - 1; i < h; ++i) {
		for (int j = 1, w = img.cols - 1; j < w; ++j) {
			if (img.at<uchar>(i, j) == 0) {
				int a[8] = {
					img.at<uchar>(i - 1, j - 1),
					img.at<uchar>(i - 1, j),
					img.at<uchar>(i - 1, j + 1),
					img.at<uchar>(i, j + 1),
					img.at<uchar>(i + 1, j + 1),
					img.at<uchar>(i + 1, j),
					img.at<uchar>(i + 1, j - 1),
					img.at<uchar>(i, j - 1)
				};
				int count = 0;
				for (int k = 0; k < 8; ++k)
					if (a[k] == 0) count += 1;
				if (count == 1) {//端点
					rst.at<uchar>(i, j) = 1; // one mark end point
				}
				if (count == 3) {//叉点
					rst.at<uchar>(i, j) = 3; //3 mark buffication point
				}
			}
		}
	}
	return rst;
}

//删除边沿伪特征点
static void deleteEdgeTriat(Mat &timg, const Mat &img, const int len = 64) {
	assert(timg.type() == CV_8UC1);
	assert(img.type() == CV_8UC1);
	assert(timg.size() == img.size());

	for (int i = 0, h = timg.rows; i < h; ++i) {
		for (int j = 0, w = timg.cols; j < w; ++j) {
			uchar &e = timg.at<uchar>(i, j);
			if (e == 1 || e == 3) {
				int a[4] = { 0 };
				for (int x = i - 1; x >= 0 && x > i - len; --x)// up
					if (img.at<uchar>(x, j) == 0) 
						a[0] += 1;
				for (int x = i + 1; x < i + len&&x < h; ++x)// down
					if (img.at<uchar>(x, j) == 0) 
						a[1] += 1;
				for (int y = j - 1; y >= 0 && y>j - len; --y)//left
					if (img.at<uchar>(i, y) == 0) 
						a[2] += 1;
				for (int y = j + 1; y < j + len&&y < w; ++y)//right
					if (img.at<uchar>(i, y) == 0) 
						a[3] += 1;

				for (int k = 0; k < 4; ++k) {
					if (a[k] == 0) 
						e = 0;
				}
			}
		}
	}
}

//删除内部伪特征点
static void deleteFakeBuffication(vector<TraitPoint> &vec, const int len = 16) {//处理断点、叉连点、小桥点
	const int N = vec.size();
	const int D = len*len;
	vector<bool> state(N, true);
	for (int i = 0; i < N; ++i) {
		if (state[i]) {//该点未标记删除
			for (int j = 0; j < N; ++j) {
				if (i != j) {//并非同一点
					int dx = abs(vec[j].x - vec[i].x);
					int dy = abs(vec[j].y - vec[i].y);
					if (dx < len&&dy < len) {
						if (dx*dx + dy*dy < D) {
							state[i] = false;
							state[j] = false;
						}
					}
				}
			}
		}
	}
	vector<TraitPoint> temp;
	for (int i = 0; i < N; ++i) {
		if (state[i])
			temp.push_back(vec[i]);
	}
	vec = temp;
}

//获取特征点集
vector<TraitPoint> getTraitPoints(const Mat &img, const Mat &ori) {
	assert(img.type() == CV_8UC1);
	const int len = 32;
	Mat _img = getTraitImage(img);
	deleteEdgeTriat(_img, img, len);
	//deleteFakeBuffication(_img, len);
	vector<TraitPoint> rst;
	for (int i = 0, h = _img.rows; i < h; ++i) {
		for (int j = 0, w = _img.cols; j < w; ++j) {
			int n = _img.at<uchar>(i, j);
			if (n == 1) rst.push_back(TraitPoint(i, j, TraitPoint::END, ori.at<uchar>(i, j)));
			if (n == 3) rst.push_back(TraitPoint(i, j, TraitPoint::BUFI, ori.at<uchar>(i, j)));
		}
	}
	//std::sort(rst.begin(), rst.end());
	deleteFakeBuffication(rst);
	return rst;
}

//测试一个点是否为边界点
static bool isEdgeTraitPoint(const Mat &img, int i, int j, const int len = 64) {
	int a[4] = { 0 };
	const int h = img.rows, w = img.cols;
	for (int x = i - 1; x >= 0 && x > i - len; --x)// up
		if (img.at<uchar>(x, j) == 0)
			a[0] += 1;
	for (int x = i + 1; x < i + len&&x < h; ++x)// down
		if (img.at<uchar>(x, j) == 0)
			a[1] += 1;
	for (int y = j - 1; y >= 0 && y>j - len; --y)//left
		if (img.at<uchar>(i, y) == 0)
			a[2] += 1;
	for (int y = j + 1; y < j + len&&y < w; ++y)//right
		if (img.at<uchar>(i, y) == 0)
			a[3] += 1;

	for (int k = 0; k < 4; ++k) {
		if (a[k] == 0)
			return true;
	}
	return false;
}
//提取特征点
vector<TraitPoint> extractTraitPoints(const Mat &img, const Mat &ori) {
	assert(img.type() == CV_8UC1);
	//Mat rst(img.size(), CV_8UC1, cv::Scalar::all(0));//zero mark point not be a trait point
	vector<TraitPoint> vec;
	for (int i = 1, h = img.rows - 1; i < h; ++i) {
		for (int j = 1, w = img.cols - 1; j < w; ++j) {
			if (img.at<uchar>(i, j) == 0) {
				int a[8] = {
					img.at<uchar>(i - 1, j - 1),
					img.at<uchar>(i - 1, j),
					img.at<uchar>(i - 1, j + 1),
					img.at<uchar>(i, j + 1),
					img.at<uchar>(i + 1, j + 1),
					img.at<uchar>(i + 1, j),
					img.at<uchar>(i + 1, j - 1),
					img.at<uchar>(i, j - 1)
				};
				int count = 0;
				for (int k = 0; k < 8; ++k)
					if (a[k] == 0) count += 1;
				if (count == 1) {//端点
					if (!isEdgeTraitPoint(img, i, j))//排除边界端点
						vec.push_back(TraitPoint(i, j, TraitPoint::END, ori.at<uchar>(i, j)));
				}
				if (count == 3) {//叉点
					vec.push_back(TraitPoint(i, j, TraitPoint::BUFI, ori.at<uchar>(i, j)));
				}
			}
		}
	}
	deleteFakeBuffication(vec);
	return vec;
}

//显示特征点，将特征点以小矩形框处，端点标为蓝色，叉点标为红色
Mat showTraitPoints(const Mat &img, const Mat &ori) {
	assert(img.type() == CV_8UC1);

	Mat sImg(img.size(), CV_8UC3, cv::Scalar::all(255));
	for (int i = 0, ni = img.rows; i < ni; ++i){
		for (int j = 0, nj = img.cols; j < nj; ++j) {
			if (img.at<uchar>(i, j) == 0) {
				sImg.at<uchar>(i, j * 3) = 0;
				sImg.at<uchar>(i, j * 3 + 1) = 0;
				sImg.at<uchar>(i, j * 3 + 2) = 0;
			}
		}
	}

	vector<TraitPoint> rst = getTraitPoints(img, ori);
	//vector<TraitPoint> extractTraitPoints(const Mat&, const Mat&);
	//vector<TraitPoint> rst = extractTraitPoints(img, ori);
	Point ps[] = {
		Point(-2, -2), Point(-2, -1), Point(-2, 0), Point(-2, 1), Point(-2, 2),
		Point(-1, 2), Point(0, 2), Point(1, 2), Point(2, 2),
		Point(2, 1), Point(2, 0), Point(2, -1), Point(2, -2),
		Point(1, -2), Point(0, -2), Point(-1, -2), Point(-2, -2) };
	for (TraitPoint &p : rst) {
		int r = 0, g = 0, b = 0;
		if (p.t == TraitPoint::END) b = 255;
		else if (p.t == TraitPoint::BUFI) r = 255;

		for (auto pp : ps) {
			int x = p.x + pp.x;
			int y = (p.y + pp.y) * 3;
			sImg.at<uchar>(x, y) = b;
			sImg.at<uchar>(x, y + 1) = g;
			sImg.at<uchar>(x, y + 2) = r;
		}
	}
	return sImg;
}

Mat showTraitPoints(const Mat &img) {
	Mat traitImg = getTraitImage(img);

	Mat sImg(traitImg.size(), CV_8UC3, cv::Scalar::all(255));
	for (int i = 0, ni = img.rows; i < ni; ++i){
		for (int j = 0, nj = img.cols; j < nj; ++j) {
			if (img.at<uchar>(i, j) == 0) {
				sImg.at<uchar>(i, j * 3) = 0;
				sImg.at<uchar>(i, j * 3 + 1) = 0;
				sImg.at<uchar>(i, j * 3 + 2) = 0;
			}
		}
	}

	const int nRow = traitImg.rows;
	const int nCol = traitImg.cols;
	Point ps[] = {
		Point(-2, -2), Point(-2, -1), Point(-2, 0), Point(-2, 1), Point(-2, 2),
		Point(-1, 2), Point(0, 2), Point(1, 2), Point(2, 2),
		Point(2, 1), Point(2, 0), Point(2, -1), Point(2, -2),
		Point(1, -2), Point(0, -2), Point(-1, -2), Point(-2, -2) };
	for (int i = 0; i < nRow; i += 1) {
		for (int j = 0; j < nCol; j += 1) {
			const uchar v = traitImg.at<uchar>(i, j);
			if (v == 1 || v == 3) {
				int r = 0, g = 0, b = 0;
				if (v == 1) b = 255;
				else if (v == 3) r = 255;

				for (const auto &p : ps) {
					int x = i + p.x;
					int y = (j + p.y) * 3;
					sImg.at<uchar>(x, y) = b;
					sImg.at<uchar>(x, y + 1) = g;
					sImg.at<uchar>(x, y + 2) = r;
				}
			}
		}
	}
	return sImg;
}