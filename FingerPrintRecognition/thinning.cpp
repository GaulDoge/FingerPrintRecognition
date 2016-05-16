#include "stdafx.h"
#include <stack>
#include <opencv2/opencv.hpp>

using std::stack;

using cv::Mat;
using cv::Point;

//细化指纹图像
void thinningImage(Mat &img, int maxIter) {
	assert(img.type() == CV_8UC1);
	int count = 0, iter = 0;
	const int one = 0, zero = 255;
	int h = img.rows - 1, w = img.cols - 1;
	do {
		count = 0;
		for (int i = 1; i < h; ++i) {
			//right-bottom corner
			for (int j = 1; j < w; ++j) {
				//condition 1
				if (img.at<uchar>(i, j) == zero) continue;

				int p[8] = {
					img.at<uchar>(i - 1, j), img.at<uchar>(i - 1, j + 1),
					img.at<uchar>(i, j + 1), img.at<uchar>(i + 1, j + 1),
					img.at<uchar>(i + 1, j), img.at<uchar>(i + 1, j - 1),
					img.at<uchar>(i, j - 1), img.at<uchar>(i - 1, j - 1)
				};
				//2
				int s = 0;
				for (int k = 0; k < 8; ++k)
					if (p[k] == one) s += 1;
				if (s < 2 || s>6) continue;
				//3
				int c = 0;
				for (int k = 0; k < 8; ++k)
					if (p[k] == zero && p[k + 1] == one)
						c += 1;
				if (p[7] == zero && p[0] == one) c += 1;
				if (c > 1) continue;
				//4
				if (p[0] * p[2] * p[4] != 0) continue;
				//5
				if (p[2] * p[4] * p[6] != 0) continue;

				//delete
				img.at<uchar>(i, j) = zero;
				count += 1;
			}
		}
		//left-top corner
		for (int i = 1; i < h; ++i) {
			for (int j = 1; j < w; ++j) {
				//condition 1
				if (img.at<uchar>(i, j) == zero) continue;

				int p[8] = {
					img.at<uchar>(i - 1, j), img.at<uchar>(i - 1, j + 1),
					img.at<uchar>(i, j + 1), img.at<uchar>(i + 1, j + 1),
					img.at<uchar>(i + 1, j), img.at<uchar>(i + 1, j - 1),
					img.at<uchar>(i, j - 1), img.at<uchar>(i - 1, j - 1)
				};
				//2
				int s = 0;
				for (int k = 0; k < 8; ++k)
					if (p[k] == one) s += 1;
				if (s < 2 || s>6) continue;
				//3
				int c = 0;
				for (int k = 0; k < 8; ++k)
					if (p[k] == zero && p[k + 1] == one)
						c += 1;
				if (p[7] == zero && p[0] == one) c += 1;
				if (c > 1) continue;
				//4
				if (p[0] * p[2] * p[6] != 0) continue;
				//5
				if (p[0] * p[4] * p[6] != 0) continue;

				//delete
				img.at<uchar>(i, j) = zero;
				count += 1;
			}
		}
		//std::cout << count << " ";
	} while (count != 0 && ++iter <= maxIter);
}

//优化细化后的指纹图像
void optimizeThinnedImage(Mat &img) {
	assert(img.type() == CV_8UC1);
	const int;
	const int;
	for (int i = 1, h = img.rows - 1; i < h; ++i) {
		for (int j = 1, w = img.cols - 1; j < w; ++j) {
			if (img.at<uchar>(i, j) == 0) {
				uchar &e1 = img.at<uchar>(i - 1, j - 1);
				uchar &e2 = img.at<uchar>(i - 1, j);
				uchar &e3 = img.at<uchar>(i - 1, j + 1);
				uchar &e4 = img.at<uchar>(i, j + 1);
				uchar &e5 = img.at<uchar>(i + 1, j + 1);
				uchar &e6 = img.at<uchar>(i + 1, j);
				uchar &e7 = img.at<uchar>(i + 1, j - 1);
				uchar &e8 = img.at<uchar>(i, j - 1);
				if (e2 == 0 && (e1 == 0 || e3 == 0)) e2 = 255;
				if (e4 == 0 && (e3 == 0 || e5 == 0)) e4 = 255;
				if (e6 == 0 && (e5 == 0 || e7 == 0)) e6 = 255;
				if (e8 == 0 && (e7 == 0 || e1 == 0)) e8 = 255;
			}
		}
	}
}

//删除孤立点和短线、毛刺
void deletePointAndShortLine(Mat &img) {
	assert(img.type() == CV_8UC1);

	int size = 16;
	auto count = [img](int i, int j) {
		std::stack<Point> st;
		if (img.at<uchar>(i - 1, j - 1) == 0) st.push(Point(i - 1, j - 1));
		if (img.at<uchar>(i - 1, j) == 0) st.push(Point(i - 1, j));
		if (img.at<uchar>(i - 1, j + 1) == 0) st.push(Point(i - 1, j + 1));
		if (img.at<uchar>(i, j - 1) == 0) st.push(Point(i, j - 1));
		if (img.at<uchar>(i, j + 1) == 0) st.push(Point(i, j + 1));
		if (img.at<uchar>(i + 1, j - 1) == 0) st.push(Point(i + 1, j - 1));
		if (img.at<uchar>(i + 1, j) == 0) st.push(Point(i + 1, j));
		if (img.at<uchar>(i + 1, j + 1) == 0) st.push(Point(i + 1, j + 1));
		return st;
	};

	for (int i = 1, in = img.rows - 1; i < in; ++i) {
		for (int j = 1, jn = img.cols - 1; j < jn; ++j) {
			if (img.at<uchar>(i, j) == 0) {
				std::stack<Point> st = count(i, j);
				if (st.size() == 0) img.at<uchar>(i, j) = 255;
				if (st.size() == 1) {
					std::stack<Point> line;
					line.push(Point(i, j));
					Point cur = st.top(); st.pop();
					while (line.size() < size) {
						st = count(cur.x, cur.y);
						if (st.size() == 2) {//当前点的8邻域有两个黑点
							Point &last = line.top();
							if (st.top() != last)
								line.push(cur), cur = st.top();
							st.pop();
							if (st.top() != last)
								line.push(cur), cur = st.top();
							st.pop();
						}
						else if (st.size() == 1) {//当前点的8邻域有一个黑点
							line.push(cur);
							break;
						}
						else break;//当前点的8邻域有2个以上的黑点
					}
					if (line.size() < size) {//脊线长度少于给定长度，将其删除
						//std::cout << line.size() << " ";
						while (!line.empty()) {
							Point &t = line.top();
							img.at<uchar>(t.x, t.y) = 255;
							line.pop();
						}
					}
				}
			}
		}
	}
}