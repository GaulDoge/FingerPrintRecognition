#include "stdafx.h"
#include "type.h"
#include <vector>
#include <opencv2/opencv.hpp>

using std::vector;

using cv::Mat;

extern vector<TraitPoint> getTraitPoints(const Mat &img, const Mat &ori);
extern vector<TraitPoint> extractTraitPoints(const Mat &img, const Mat &ori);

//计算局部特征向量
vector<LocalTrait> getLocalTrait(const Mat img, const vector<TraitPoint> va) {
	const int na = va.size();
	vector<LocalTrait> vsa(na);
	for (int i = 0; i < na; ++i) {
		TraitPoint itp = va[i];
		int ia = 0, da = 480 * 480;
		int ib = 0, db = 480 * 480;
		for (int j = 0; j < na; ++j){
			TraitPoint jtp = va[j];
			int dx = jtp.x - itp.x;
			int dy = jtp.y - itp.y;
			int d = dx*dx + dy*dy;
			if (i == j) continue;
			else if (d < da) ia = j, da = d;
			else if (d < db) ib = j, db = d;
		}
		vsa[i].p[0] = ia; vsa[i].p[1] = ib;//p0 p1
		vsa[i].t[0] = va[ia].t; vsa[i].t[1] = va[ib].t;//t0 t1
		vsa[i].d[0] = sqrt(da); vsa[i].d[1] = sqrt(db);//d0 d1
		{//d2
			int dx = va[ia].x - va[ib].x;
			int dy = va[ia].y - va[ib].y;
			vsa[i].d[2] = sqrt(dx*dx + dy*dy);
		}
		auto getRCount = [va, img](int a, int b) -> int {//求两点连线穿过的脊线数目
			int count = 0;
			int x1 = va[a].x, x2 = va[b].x;
			int y1 = va[a].y, y2 = va[b].y;
			if (x1 == x2) {//垂直，没有斜率
				if (y1 > y2) {
					int t = y1; y1 = y2; y2 = t;
				}
				int x = x1, y = y1;
				while (++y < y2) {
					if (img.at<uchar>(x, y) == 0)
						count += 1;
				}
			}
			else {//其它
				if (x1 > x2) {
					int t = x1; x1 = x2; x2 = t;
					t = y1; y1 = y2; y2 = t;
				}
				int x = x1;
				float y = y1;
				float k = (y1 - y2) / (x1 - x2);
				while (++x < x2) {
					y += k;
					if (img.at<uchar>(x, floor(y)) == 0)
						count += 1;
				}
			}
			return count;
		};
		vsa[i].r[0] = getRCount(i, ia);
		vsa[i].r[1] = getRCount(i, ib);
		vsa[i].r[2] = getRCount(ib, ia);
	}
	return vsa;
}

//初匹配
vector<int> matchStep1(
	Mat aimg, const vector<TraitPoint> va, 
	Mat bimg, const vector<TraitPoint> vb) {
	const int an = va.size();
	const int bn = vb.size();
	const int dd = 16, dr = 4;
	vector<LocalTrait> avec = getLocalTrait(aimg, va);
	vector<LocalTrait> bvec = getLocalTrait(bimg, vb);
	vector<vector<bool>> stat(an, vector<bool>(bn));
	for (int i = 0; i < an; ++i) {//测试点的匹配
		for (int j = 0; j < bn; ++j) {
			int d = 0;
			for (int k = 0; k < 3; ++k)
				d += abs(avec[i].d[k] - bvec[j].d[k]);
			int r = 0;
			for (int k = 0; k < 3; ++k)
				r += abs(avec[i].r[k] - bvec[j].r[k]);
			if (va[i].t == vb[j].t && // condition 1
				d < dd&&r < dr && //condition 2
				(avec[i].t[0] == bvec[j].t[0] && avec[i].t[1] == bvec[j].t[1] ||//condition 3
				avec[i].t[0] == bvec[j].t[1] && avec[i].t[1] == bvec[j].t[0]))
				stat[i][j] = true;
			else stat[i][j] = false;
		}
	}
	int count = 0;
	for (const auto &e1 : stat)
		for (const auto &e : e1)
			if (e) count += 1;
	//float rate = 1.0 * count * count / an / bn;
	//float rate = float(count) / (an < bn ? an : bn);
	//cout << count << " " << rate << endl;
	/*if (rate > 0.3)
		return true;
	else return false;*/
	vector<int> result(3);
	result[0] = an; 
	result[1] = bn;
	result[2] = count;
	return result;
}

vector<int> matchStep1(
	vector<TraitPoint> va, vector<LocalTrait> avec,
	vector<TraitPoint> vb, vector<LocalTrait> bvec) {
	const int an = va.size();
	const int bn = vb.size();
	const int dd = 16, dr = 4;
	vector<vector<bool>> stat(an, vector<bool>(bn));
	for (int i = 0; i < an; ++i) {//测试点的匹配
		for (int j = 0; j < bn; ++j) {
			int d = 0;
			for (int k = 0; k < 3; ++k)
				d += abs(avec[i].d[k] - bvec[j].d[k]);
			int r = 0;
			for (int k = 0; k < 3; ++k)
				r += abs(avec[i].r[k] - bvec[j].r[k]);
			if (va[i].t == vb[j].t && // condition 1
				d < dd&&r < dr && //condition 2
				(avec[i].t[0] == bvec[j].t[0] && avec[i].t[1] == bvec[j].t[1] ||//condition 3
				avec[i].t[0] == bvec[j].t[1] && avec[i].t[1] == bvec[j].t[0]))
				stat[i][j] = true;
			else stat[i][j] = false;
		}
	}
	int count = 0;
	for (const auto &e1 : stat)
		for (const auto &e : e1)
			if (e) count += 1;
	vector<int> result(3);
	result[0] = an;
	result[1] = bn;
	result[2] = count;
	return result;
}

//再匹配
vector<int> matchStep2(Mat aimg, const vector<TraitPoint> va, Mat bimg, const vector<TraitPoint> vb) {
	return vector<int>(3,0);
}

vector<int> match(Mat aimg, Mat aori, Mat bimg, Mat bori) {
	vector<TraitPoint> avec = getTraitPoints(aimg, aori);
	vector<TraitPoint> bvec = getTraitPoints(bimg, aori);
	return matchStep1(aimg, avec, bimg, bvec);
}

vector<int> match(
	vector<TraitPoint> va, vector<LocalTrait> avec,
	vector<TraitPoint> vb, vector<LocalTrait> bvec) {
	return matchStep1(va, avec, vb, bvec);
}
