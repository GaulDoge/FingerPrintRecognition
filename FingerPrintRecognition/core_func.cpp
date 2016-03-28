#include<opencv2/opencv.hpp>
#include "stdafx.h"
#include "core_func.h"

using cv::Mat;

//finger print image preprocess
void preprocess(Mat &img) {

}

static Mat direct_img(Mat &img) {
	return Mat();
}

static void direct_img_denosing(Mat &img) {

}

static void direct_filter(Mat &img) {

}

static void direct_filter_denosing(Mat &img) {

}

//finger print image trait extraction
void extract_trait(Mat &img) {

}

//test two finger print image match or not
bool fp_match(const Mat &imga, const Mat &imgb) {
	return true;
}

//recognize a finger print in database
void fp_recognize(const Mat &img) {

}