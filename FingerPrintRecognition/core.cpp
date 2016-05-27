#include "stdafx.h"
#include "core.h"
#include <fstream>
#include <vector>
#include <exception>
using std::vector;
using std::exception;

void thresholdImage(cv::Mat &nor) {
	cv::adaptiveThreshold(nor, nor, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17, 1);
	thresholdImageDenosing(nor);
}

void preprocess(cv::Mat &nor, cv::Mat &ori) {
	 ori = directionalOrientation(nor);
	 toBlockOrientation(ori);
	 nor = directionalFilter(nor, ori);
	 thresholdImage(nor);
	 thinningImage(nor);
	 optimizeThinnedImage(nor);
	 deletePointAndShortLine(nor);
}

static bool writeTraitPoint(std::ofstream &out, const TraitPoint &p) {
	int x = p.x;
	int y = p.y;
	int t = p.t;
	int d = p.d;
	try {
		out.write((char*)(&x), sizeof(x));
		out.write((char*)(&y), sizeof(y));
		out.write((char*)(&t), sizeof(t));
		out.write((char*)(&d), sizeof(d));
		return true;
	}
	catch (exception &e) {
		return false;
	}
}

static bool readTraitPoint(std::ifstream &in, TraitPoint &tp) {
	int x, y, t, d;
	try {
		in.read((char*)(&x), sizeof(x));
		in.read((char*)(&y), sizeof(y));
		in.read((char*)(&t), sizeof(t));
		in.read((char*)(&d), sizeof(d));
		tp = TraitPoint(x, y, t, d);
		return true;
	}
	catch (exception &e) {
		return false;
	}
}

static bool writeLocalTrait(std::ofstream &out, const LocalTrait lt) {
	try {
		out.write((char*)(&lt.p), sizeof(lt.p));
		out.write((char*)(&lt.t), sizeof(lt.t));
		out.write((char*)(&lt.d), sizeof(lt.d));
		out.write((char*)(&lt.r), sizeof(lt.r));
		return true;
	}
	catch (exception &e) {
		return false;
	}
}

static bool readLocalTrait(std::ifstream &in, LocalTrait &lt) {
	try {
		in.read((char*)(&lt.p), sizeof(lt.p));
		in.read((char*)(&lt.t), sizeof(lt.t));
		in.read((char*)(&lt.d), sizeof(lt.d));
		in.read((char*)(&lt.r), sizeof(lt.r));
		return true;
	}
	catch (exception &e) {
		return false;
	}
}

bool writeTraits(std::ofstream &out, std::vector<TraitPoint> tp, std::vector<LocalTrait> lt) {
	if (!out.is_open()) {
		return false;
	}

	try {
		const int size1 = tp.size();
		out.write((char*)(&size1), sizeof(size1));
		for (const auto &e : tp) {
			writeTraitPoint(out, e);
		}

		const int size2 = lt.size();
		out.write((char*)(&size2), sizeof(size2));
		for (const auto &e : lt) {
			writeLocalTrait(out, e);
		}

		return true;
	} 
	catch (exception &e) {
		return false;
	}
}

vector<int> recognize(
	std::ifstream &in,
	std::vector<TraitPoint> va,
	std::vector<LocalTrait> avec) {

	if (!in.is_open()) {
		return vector<int>();
	}

	int size1;
	vector<TraitPoint> vb;
	in.read((char*)(&size1), sizeof(size1));
	for (int i=0;i<size1; i+=1) {
		TraitPoint t;
		if (readTraitPoint(in, t)) {
			vb.push_back(t);
		}
		else {
			break;
		}
	}

	int size2;
	vector<LocalTrait> bvec;
	in.read((char*)(&size2), sizeof(size2));
	for (int i=0; i<size2; i+=1) {
		LocalTrait lt;
		if (readLocalTrait(in, lt)) {
			bvec.push_back(lt);
		}
		else {
			break;
		}
	}

	if (vb.size() != size1 || bvec.size() != size2) {
		return vector<int>();
	}
	else {
		return match(va, avec, vb, bvec);
	}
}