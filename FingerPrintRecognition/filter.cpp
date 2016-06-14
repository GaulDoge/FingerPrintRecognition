#include "stdafx.h"
#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Point;
using cv::Range;

//规格化，裁剪和灰度化指纹图像
Mat normalize(const Mat &img) {
	int n = img.rows, m = img.cols;
	int nl, nu;
	if (n > 480) nl = (n - 480) / 2;
	else if (n > 320) nl = (n - 320) / 2;
	else if (n > 256) nl = (n - 256) / 2;
	nu = n - nl;

	int  ml, mu;
	if (m > 480) ml = (m - 480) / 2;
	else if (m > 320) ml = (m - 320) / 2;
	else if (m > 256) ml = (m - 256) / 2;
	mu = m - ml;

	Mat nor(nu - nl, mu - ml, CV_8UC1);
	cv::cvtColor(img(Range(nl, nu), Range(ml, mu)), nor, CV_BGR2GRAY);

	return nor;
}

//计算方向图
const int _n = 9;
extern int _oritpt[8][9][2];
inline static int pointAt(int d, int o, int w) {
	return _oritpt[d][o][w];
}
Mat directionalOrientation(const Mat &nor) {
	assert(nor.type() == CV_8UC1);

	//计算一个方向上5个像素的灰度值的方差
	//d -> 方向, (x, y)中心点坐标
	auto getVar = [nor](int d, int x, int y)->int {
		int tb[_n] = { 0 };
		for (int i = 0; i < _n; ++i)
			tb[i] = nor.at<uchar>(x + pointAt(d, i, 0), y + pointAt(d, i, 1));

		int M = 0;
		for (int i = 0; i < _n; ++i)
			M += tb[i];
		M /= _n;

		int Var = 0;
		for (int i = 0; i < _n; ++i)
			//Var += (M - tb[i]) * (M - tb[i]);
			Var += abs(M - tb[i]);

		return Var / _n;
	};

	int border = 8;
	const int nRow = nor.rows, nCol = nor.cols;
	Mat ori(nor.size(), nor.type(), cv::Scalar::all(0));
	for (int i = border; i < nRow - border; ++i) {
		for (int j = border; j < nCol - border; ++j) {
			int a[8] = { 0 };
			for (int d = 0; d < 8; ++d)
				a[d] = getVar(d, i, j);
			int min = 0;
			for (int d = 1; d < 8; ++d)
				if (a[d] < a[min]) min = d;
			ori.at<uchar>(i, j) = min;
		}
	}

	return ori;
}

//方向图块化
void toBlockOrientation(Mat &ori) {
	assert(ori.type() == CV_8UC1);

	const int step = 8;
	const int nRow = ori.rows, nCol = ori.cols;
	Mat bori(ori.size(), ori.type(), cv::Scalar::all(0));
	for (int i = 0; i < nRow - step; i += step/2) {
		for (int j = 0; j < nCol - step; j += step/2) {
			int a[8] = { 0 };
			for (int x = 0; x < step; ++x) {
				for (int y = 0; y < step; ++y) {
					a[ori.at<uchar>(i + x, y + j)] += 1;
				}
			}

			int max = 0;
			for (int k = 1; k < 8; ++k) {
				if (a[k] > a[max])
					max = k;
			}

			bori(Range(i, i + step), Range(j, j + step)) = max;
		}
	}
	ori = bori;
}

//方向滤波
extern int _smooth[8][7][7];
extern int _sharp[8][7][7];
inline static int getSmooth(int d, int x, int y) {
	return _smooth[d][x][y];
}
inline static int getSharp(int d, int x, int y) {
	return _sharp[d][x][y];
}
Mat directionalFilter(const Mat &nor, const Mat &ori) {
	assert(nor.type() == CV_8UC1);
	assert(ori.type() == CV_8UC1);

	const int nRow = nor.rows, nCol = nor.cols;
	Mat rst(nRow, nCol, CV_8UC1, cv::Scalar::all(255));
	//平滑滤波
	for (int i = 3; i < nRow - 3; ++i) {
		for (int j = 3; j < nCol - 3; ++j) {
			const int d = ori.at<uchar>(i, j);
			//卷积运算
			int value = 0, n = 0;
			for (int x = -3; x < 4; ++x) {
				for (int y = -3; y < 4; ++y) {
					int t = getSmooth(d, x + 3, y + 3);
					value += nor.at<uchar>(i + x, j + y) * t;
					n += t;
				}
			}
			rst.at<uchar>(i, j) = cv::saturate_cast<uchar>(value / n);
		}
	}
	//return rst;
	//锐化滤波
	//Mat rst1(LEN, LEN, CV_8UC1, cv::Scalar::all(0));
	for (int i = 3; i < nRow - 3; ++i) {
		for (int j = 3; j < nCol - 3; ++j) {
			const int d = ori.at<uchar>(i, j);
			//卷积运算
			int value = 0, n = 0;
			for (int x = -3; x < 4; ++x) {
				for (int y = -3; y < 4; ++y){
					int t = getSharp(d, x + 3, y + 3);
					value += rst.at<uchar>(i + x, j + y) * t;
					n += t;
				}
			}
			rst.at<uchar>(i, j) = cv::saturate_cast<uchar>(value / n);
		}
	}

	return rst;
}

//二值化图像去燥（主要去除脊线上的气泡，有时间可以加入其它优化）
void thresholdImageDenosing(Mat &img) {
	assert(img.type() == CV_8UC1);

	for (int i = 1, in = img.rows - 1; i < in; ++i) {
		for (int j = 1, jn = img.cols - 1; j < jn; ++j) {
			uchar &front = img.at<uchar>(i, j);
			const uchar back = 255 - front;
			int c = 0;
			for (int x = -1; x < 2; ++x) {
				for (int y = -1; y < 2; ++y) {
					if (img.at<uchar>(x + i, y + j) == back)
						c += 1;
				}
			}
			if (c >= 5) front = 0;
		}
	}
}



static int _oritpt[8][9][2] = {
	{ { 0, 8 }, { 0, 6 }, { 0, 4 }, { 0, 2 }, { 0, 0 }, { 0, -2 }, { 0, -4 }, { 0, -6 }, { 0, -8 } },
	{ { -4, 8 }, { -3, 6 }, { -2, 4 }, { -1, 2 }, { 0, 0 }, { 1, -2 }, { 2, -4 }, { 3, -6 }, { 4, -8 } },
	{ { -8, 8 }, { -6, 6 }, { -4, 4 }, { -2, 2 }, { 0, 0 }, { 2, -2 }, { 4, -4 }, { 6, -6 }, { 8, -8 } },
	{ { -8, 4 }, { -6, 3 }, { -4, 2 }, { -2, 1 }, { 0, 0 }, { 2, -1 }, { 4, -2 }, { 6, -3 }, { 8, -4 } },
	{ { -8, 0 }, { -6, 0 }, { -4, 0 }, { -2, 0 }, { 0, 0 }, { 2, 0 }, { 4, 0 }, { 6, 0 }, { 8, 0 } },
	{ { -8, -4 }, { -6, -3 }, { -4, -2 }, { -2, -1 }, { 0, 0 }, { 2, 1 }, { 4, 2 }, { 6, 3 }, { 8, 4 } },
	{ { -8, -8 }, { -6, -6 }, { -4, -4 }, { -2, -4 }, { 0, 0 }, { 2, 2 }, { 4, 4 }, { 6, 6 }, { 8, 8 } },
	{ { -4, -8 }, { -3, -6 }, { -2, -4 }, { -1, -2 }, { 0, 0 }, { 1, 2 }, { 2, 4 }, { 3, 6 }, { 4, 8 } }
};

static int _smooth[8][7][7] = {
	{//1
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 1, 1, 1, 1 },
		{ 5, 5, 5, 5, 5, 5, 5 },
		{ 8, 8, 8, 8, 8, 8, 8 },
		{ 5, 5, 5, 5, 5, 5, 5 },
		{ 1, 1, 1, 1, 1, 1, 1 },
		{ 0, 0, 0, 0, 0, 0, 0 }
	},
	{//2
		{ 0, 0, 0, 0, 0, 1, 1 },
		{ 0, 0, 1, 1, 1, 5, 5 },
		{ 1, 1, 5, 5, 5, 8, 8 },
		{ 5, 5, 8, 8, 8, 5, 5 },
		{ 8, 8, 5, 5, 5, 1, 1 },
		{ 5, 5, 1, 1, 1, 0, 0 },
		{ 1, 1, 0, 0, 0, 0, 0 }
	},
	{//3
		{ 0, 0, 0, 0, 1, 5, 8 },
		{ 0, 0, 0, 1, 5, 8, 5 },
		{ 0, 0, 1, 5, 8, 5, 1 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 1, 5, 8, 5, 1, 0, 0 },
		{ 5, 8, 5, 1, 0, 0, 0 },
		{ 8, 5, 1, 0, 0, 0, 0 }
	},
	{//4
		{ 0, 0, 1, 5, 8, 5, 1 },
		{ 0, 0, 1, 5, 8, 5, 1 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 1, 5, 8, 5, 1, 0, 0 },
		{ 1, 5, 8, 5, 1, 0, 0 }
	},
	{//5
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 }
	},
	{//6
		{ 1, 5, 8, 5, 1, 0, 0 },
		{ 1, 5, 8, 5, 1, 0, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 0, 1, 5, 8, 5, 1 },
		{ 0, 0, 1, 5, 8, 5, 1 }
	},
	{//7
		{ 8, 5, 1, 0, 0, 0, 0 },
		{ 5, 8, 5, 1, 0, 0, 0 },
		{ 1, 5, 8, 5, 1, 0, 0 },
		{ 0, 1, 5, 8, 5, 1, 0 },
		{ 0, 0, 1, 5, 8, 5, 1 },
		{ 0, 0, 0, 1, 5, 8, 5 },
		{ 0, 0, 0, 0, 1, 5, 8 }
	},
	{//8
		{ 1, 1, 0, 0, 0, 0, 0 },
		{ 5, 5, 1, 1, 1, 0, 0 },
		{ 8, 8, 5, 5, 5, 1, 1 },
		{ 5, 5, 8, 8, 8, 5, 5 },
		{ 1, 1, 5, 5, 5, 8, 8 },
		{ 0, 0, 1, 1, 1, 5, 5 },
		{ 0, 0, 0, 0, 0, 1, 1 }
	}
};

static int _sharp[8][7][7] = {
	{//1
		{ -3, -3, -3, -3, -3, -3, -3 },
		{ 2, 2, 2, 2, 2, 2, 2 },
		{ 6, 6, 6, 6, 6, 6, 6 },
		{ 10, 10, 10, 10, 10, 10, 10 },
		{ 6, 6, 6, 6, 6, 6, 6 },
		{ 2, 2, 2, 2, 2, 2, 2 },
		{ -3, -3, -3, -3, -3, -3, -3 }
	},
	{//2
		{ 0, 0, -3, -3, -3, 2, 2 },
		{ -3, -3, 2, 2, 2, 6, 6 },
		{ 2, 2, 6, 6, 6, 10, 10 },
		{ 6, 6, 10, 10, 10, 6, 6 },
		{ 10, 10, 6, 6, 6, 2, 2 },
		{ 6, 6, 2, 2, 2, -3, -3 },
		{ 2, 2, -3, -3, -3, 0, 0 }
	},
	{//3
		{ 0, 0, 0, -3, 2, 6, 10 },
		{ 0, 0, -3, 2, 6, 10, 6 },
		{ 0, -3, 2, 6, 10, 6, 2 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ 2, 6, 10, 6, 2, -3, 0 },
		{ 6, 10, 6, 2, -3, 0, 0 },
		{ 10, 6, 2, -3, 0, 0, 0 }
	},
	{//4
		{ 0, -3, 2, 6, 10, 6, 2 },
		{ 0, -3, 2, 6, 10, 6, 2 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ 2, 6, 10, 6, 2, -3, 0 },
		{ 2, 6, 10, 6, 2, -3, 0 }
	},
	{//5
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ -3, 2, 6, 10, 6, 2, -3 }
	},
	{//6
		{ 2, 6, 10, 6, 2, -3, 0 },
		{ 2, 6, 10, 6, 2, -3, 0 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ 0, -3, 2, 6, 10, 6, 2 },
		{ 0, -3, 2, 6, 10, 6, 2 }
	},
	{//7
		{ 10, 6, 2, -3, 0, 0, 0 },
		{ 6, 10, 6, 2, -3, 0, 0 },
		{ 2, 6, 10, 6, 2, -3, 0 },
		{ -3, 2, 6, 10, 6, 2, -3 },
		{ 0, -3, 2, 6, 10, 6, 2 },
		{ 0, 0, -3, 2, 6, 10, 6 },
		{ 0, 0, 0, -3, 2, 6, 10 }
	},
	{//8
		{ 2, 2, -3, -3, -3, 0, 0 },
		{ 6, 6, 2, 2, 2, -3, -3 },
		{ 10, 10, 6, 6, 6, 2, 2 },
		{ 6, 6, 10, 10, 10, 6, 6 },
		{ 2, 2, 6, 6, 6, 10, 10 },
		{ -3, -3, 2, 2, 2, 6, 6 },
		{ 0, 0, -3, -3, -3, 2, 2 }
	}
};
