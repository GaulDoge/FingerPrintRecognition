#include<opencv2/opencv.hpp>

extern void preprocess(cv::Mat&);

extern void extract_trait(cv::Mat&);

extern bool fp_match(const cv::Mat&, const cv::Mat&);

extern void fp_recognize(const cv::Mat&);