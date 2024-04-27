
#include "utility.h"

Mat loadImage(string path) {
	Mat image = imread(path, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "ERROR : Image cannot be loaded..!!" << endl;
		return Mat();
	}
	return image;
}

std::vector<cv::Rect> getBoundingBoxes(cv::Mat image) {
	//Find contours on the thresholded PSD image and get its bounding box
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(image, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	// Get the bounding box of each contour
	std::vector<cv::Rect> boundingBoxes(contours.size());
	for (size_t i = 0; i < contours.size(); i++) {
		boundingBoxes[i] = cv::boundingRect(contours[i]);
	}

	return boundingBoxes;
}

std::vector<cv::Rect> getValidBoundingBoxes(cv::Mat image, int minArea) {
	std::vector<cv::Rect> boundingBoxes = getBoundingBoxes(image);
	boundingBoxes.erase(std::remove_if(boundingBoxes.begin(), boundingBoxes.end(), [minArea](const cv::Rect& box) {
		return box.area() < minArea;
		}), boundingBoxes.end());

	// Remove biggest box
	boundingBoxes.erase(std::max_element(boundingBoxes.begin(), boundingBoxes.end(), [](const cv::Rect& box1, const cv::Rect& box2) {
		return box1.area() < box2.area();
		}));

	return boundingBoxes;
}

void makeBoxesSquare(std::vector<cv::Rect>& boundingBoxes) {
	// Make boxes square. Smaller dimension is increased to match the larger one
	for (auto& box : boundingBoxes) {
		int diff = box.width - box.height;
		if (diff > 0) {
			box.y -= diff / 2;
			box.height += diff;
		}
		else {
			box.x -= -diff / 2;
			box.width += -diff;
		}
	}
}

void displayBoxes(cv::Mat image, std::vector<cv::Rect> boundingBoxes) {
	cv::Mat colorImage;
	cv::cvtColor(image, colorImage, cv::COLOR_GRAY2BGR);

	for (const auto& box : boundingBoxes) {
		cv::rectangle(colorImage, box, cv::Scalar(0, 0, 255), 2);
	}

	cv::imshow("Image with bounding boxes", colorImage);
}


void makeImageSizeEven(cv::Mat& image) {
	Rect roi = Rect(0, 0, image.cols & -2, image.rows & -2);
	image = image(roi);
}


