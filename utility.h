#pragma once

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat loadImage(string path);

std::vector<cv::Rect> getBoundingBoxes(cv::Mat image);

std::vector<cv::Rect> getValidBoundingBoxes(cv::Mat image, int minArea = 10);

void makeBoxesSquare(std::vector<cv::Rect>& boundingBoxes);

void displayBoxes(cv::Mat image, std::vector<cv::Rect> boundingBoxes);

void makeImageSizeEven(cv::Mat& image);
