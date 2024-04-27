
#include "imageProcessing.h"

using namespace cv;


void fftshift(const Mat& inputImg, Mat& outputImg)
{
	outputImg = inputImg.clone();
	int cx = outputImg.cols / 2;
	int cy = outputImg.rows / 2;
	Mat q0(outputImg, Rect(0, 0, cx, cy));
	Mat q1(outputImg, Rect(cx, 0, cx, cy));
	Mat q2(outputImg, Rect(0, cy, cx, cy));
	Mat q3(outputImg, Rect(cx, cy, cx, cy));
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
}

void filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H)
{
	Mat planes[2] = { Mat_<float>(inputImg.clone()), Mat::zeros(inputImg.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);
	dft(complexI, complexI, DFT_SCALE);

	Mat planesH[2] = { Mat_<float>(H.clone()), Mat::zeros(H.size(), CV_32F) };
	Mat complexH;
	merge(planesH, 2, complexH);
	Mat complexIH;
	mulSpectrums(complexI, complexH, complexIH, 0);

	idft(complexIH, complexIH);
	split(complexIH, planes);
	outputImg = planes[0];
}

void synthesizeFilterH(Mat& inputOutput_H, Point center, int radius)
{
	Point c2 = center, c3 = center, c4 = center;
	c2.y = inputOutput_H.rows - center.y;
	c3.x = inputOutput_H.cols - center.x;
	c4 = Point(c3.x, c2.y);
	circle(inputOutput_H, center, radius, 0, -1, 8);
	circle(inputOutput_H, c2, radius, 0, -1, 8);
	circle(inputOutput_H, c3, radius, 0, -1, 8);
	circle(inputOutput_H, c4, radius, 0, -1, 8);
}

void synthesizeFilterG(Mat& inputOutput_G, Point center, int radius)
{
	// Create a Gaussian mask
	Mat gaussianMask = Mat(inputOutput_G.size(), CV_32F);
	for (int i = 0; i < gaussianMask.rows; i++) {
		for (int j = 0; j < gaussianMask.cols; j++) {
			double distance = norm(Point(j, i) - center);
			gaussianMask.at<float>(i, j) = exp(-(distance * distance) / (2 * radius * radius));
		}
	}

	// Apply the Gaussian mask to the image
	multiply(inputOutput_G, gaussianMask, inputOutput_G);
}

// Function calculates PSD(Power spectrum density) by fft with two flags
// flag = 0 means to return PSD
// flag = 1 means to return log(PSD)
void calcPSD(const Mat& inputImg, Mat& outputImg, int flag)
{
	Mat planes[2] = { Mat_<float>(inputImg.clone()), Mat::zeros(inputImg.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);
	dft(complexI, complexI);
	split(complexI, planes); // planes[0] = Re(DFT(I)), planes[1] = Im(DFT(I))

	planes[0].at<float>(0) = 0;
	planes[1].at<float>(0) = 0;

	// compute the PSD = sqrt(Re(DFT(I))^2 + Im(DFT(I))^2)^2
	Mat imgPSD;
	magnitude(planes[0], planes[1], imgPSD); //imgPSD = sqrt(Power spectrum density)
	pow(imgPSD, 2, imgPSD); //it needs ^2 in order to get PSD
	outputImg = imgPSD;

	// logPSD = log(1 + PSD)
	if (flag)
	{
		Mat imglogPSD;
		imglogPSD = imgPSD + Scalar::all(1);
		log(imglogPSD, imglogPSD);
		outputImg = imglogPSD;
	}
}

void findAndDisplayContours(cv::Mat grayImage) {
	// Użyj binaryzacji, aby przekształcić obraz na obraz binarny
	cv::Mat binary;
	cv::threshold(grayImage, binary, 225, 255, cv::THRESH_BINARY_INV);

	// Znajdź kontury na obrazie binarnym
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(binary, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	// Narysuj kontury na oryginalnym obrazie
	cv::Mat contourImage = cv::Mat::zeros(grayImage.size(), CV_8UC3);
	cv::drawContours(contourImage, contours, -1, cv::Scalar(0, 255, 0), 3);

	// Wyświetl obraz z konturami
	cv::imshow("Image with contours", contourImage);
	cv::waitKey(0);
}

Mat getPSD(const cv::Mat& image, int flag) {
	Mat imgPSD;
	calcPSD(image, imgPSD, flag);
	fftshift(imgPSD, imgPSD);
	normalize(imgPSD, imgPSD, 0, 255, NORM_MINMAX);
	return imgPSD;
}

void applyMorphologicalClosing(cv::Mat& image, int closingSize = 4, int dilateSize = 2) {
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * closingSize + 1, 2 * closingSize + 1), cv::Point(closingSize, closingSize));
	cv::morphologyEx(image, image, cv::MORPH_CLOSE, element);
	cv::dilate(image, image, element, cv::Point(-1, -1), dilateSize);
}

Mat getGaussianFilter(cv::Size sizeImage, std::vector<cv::Rect>& boundingBoxes) {
	cv::Mat G = cv::Mat::ones(sizeImage, CV_32F);
	// Create a Gaussian mask
	for (auto& box : boundingBoxes) {
		cv::Point center = cv::Point(box.x + box.width / 2, box.y + box.height / 2);
		int r = std::max(box.width, box.height) / 2;

		// get part of image that have center in the box
		cv::Rect enlargedBox(center.x - 3 * r, center.y - 3 * r, 6 * r, 6 * r);
		cv::Mat part = G(enlargedBox);

		// multiply image part by gaussian mask
		for (int i = 0; i < part.rows; i++) {
			for (int j = 0; j < part.cols; j++) {
				double distance = cv::norm(cv::Point(j, i) - cv::Point(part.cols / 2, part.rows / 2));
				part.at<float>(i, j) *= (1 - exp(-(distance * distance) / (2 * r * r)));
			}
		}
	}
	return G;
}

cv::Mat getHFilter(cv::Size sizeImage, std::vector<cv::Rect> boundingBoxes) {
	Mat H = Mat(sizeImage, CV_32F, Scalar(1));

	for (auto& box : boundingBoxes) {
		Point center = Point(box.x + box.width / 2, box.y + box.height / 2);
		int r = std::max(box.width, box.height) / 2;
		synthesizeFilterH(H, center, r);
	}
	return H;
}

void applyPSDFilter(const Mat& inputImage, Mat& outputImage, Mat filter) {
	fftshift(filter, filter);
	filter2DFreq(inputImage, outputImage, filter);
}
