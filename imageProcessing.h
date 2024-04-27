#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
	
void fftshift(const cv::Mat& inputImg, cv::Mat& outputImg);
void filter2DFreq(const cv::Mat& inputImg, cv::Mat& outputImg, const cv::Mat& H);
void synthesizeFilterH(cv::Mat& inputOutput_H, cv::Point center, int radius);
void synthesizeFilterG(cv::Mat& inputOutput_G, cv::Point center, int radius);
void calcPSD(const cv::Mat& inputImg, cv::Mat& outputImg, int flag = 0);

// Function calculates PSD(Power spectrum density)
cv::Mat getPSD(const cv::Mat& image, int flag = 0);

// Function applies morphological closing to the input image
void applyMorphologicalClosing(cv::Mat& image, int kernelSize, int iterations);

// Function calculates the filter H
cv::Mat getHFilter(cv::Size sizeImage, std::vector<cv::Rect> boundingBoxes);

// Function applies the PSD filter to the input image
void applyPSDFilter(const cv::Mat& inputImage, cv::Mat& outputImage, cv::Mat filter);

cv::Mat getGaussianFilter(cv::Size sizeImage, std::vector<cv::Rect>& boundingBoxes);
