// Created by: Kamil Kalinowski

/*
 * This is a modified version of the code found at https://docs.opencv.org/4.x/d2/d0b/tutorial_periodic_noise_removing_filter.html
 * The purpose of this modification is to test the effect of replacing the sharp cut-off circle filter with a Gaussian function filter on noise removal.
 * Additionally, the process of cutting off frequencies responsible for periodic noise has been automated.
 */

#include "imageProcessing.h"
#include "utility.h"

using namespace cv;
using namespace std;


int main(int argc, char* argv[])
{
	string strInFileName = "materials/period_input.jpg";

	Mat imgIn = loadImage(strInFileName);
	if (imgIn.empty()) {
		return -1;
	}

	makeImageSizeEven(imgIn);

	imshow("Image input", imgIn);

	Mat imgPSD = getPSD(imgIn);

	imshow("PSD", imgPSD);

	Mat imgPSD_8U;
	imgPSD.convertTo(imgPSD_8U, CV_8U);


	Mat imgThresholded;
	threshold(imgPSD_8U, imgThresholded, 0, 255, THRESH_BINARY);


	applyMorphologicalClosing(imgThresholded, 5, 1);

	imshow("Thresholded PSD", imgThresholded);

	// Find contours on the thresholded PSD image and get its bounding box
	std::vector<cv::Rect> boundingBoxes = getValidBoundingBoxes(imgThresholded, 0);

	makeBoxesSquare(boundingBoxes);

	displayBoxes(imgThresholded, boundingBoxes);



	Mat H = getHFilter(imgPSD.size(), boundingBoxes);
	imshow("Filter H", H);

	Mat G = getGaussianFilter(imgIn.size(), boundingBoxes);
	imshow("Filter G", G);


	Mat imgOutH; imgIn.copyTo(imgOutH);
	applyPSDFilter(imgOutH, imgOutH, H);
	imgOutH.convertTo(imgOutH, CV_8U);
	normalize(imgOutH, imgOutH, 0, 255, NORM_MINMAX);
	imshow("Debluring H", imgOutH);

	Mat imgOutG; imgIn.copyTo(imgOutG);
	applyPSDFilter(imgOutG, imgOutG, G);
	imgOutG.convertTo(imgOutG, CV_8U);
	normalize(imgOutG, imgOutG, 0, 255, NORM_MINMAX);
	imshow("Debluring G", imgOutG);


	normalize(G, G, 0, 255, NORM_MINMAX);
	imwrite("results/FilterG.jpg", G);

	normalize(H, H, 0, 255, NORM_MINMAX);
	imwrite("results/FilterH.jpg", H);

	imwrite("results/DebluringH.jpg", imgOutH);
	imwrite("results/DebluringG.jpg", imgOutG);

	imgPSD = getPSD(imgIn, 1);
	imgPSD.convertTo(imgPSD, CV_8U);
	normalize(imgPSD, imgPSD, 0, 255, NORM_MINMAX);
	imwrite("results/PSD.jpg", imgPSD);



	cv::waitKey(0);
	return 0;
}
