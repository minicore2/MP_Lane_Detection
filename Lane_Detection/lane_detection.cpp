#include <array>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>
#include <sstream>
#include <iomanip>

#include "..\tinyxml2\tinyxml2.h"
#include "gpx.h"
#include "camera.h"

#if defined( _MSC_VER ) || defined (WIN32)
#include <crtdbg.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace tinyxml2;
using namespace std;

void putTextOnImage(std::string text, cv::Mat image) {
	std::size_t posEnd = text.find("\n");
	std::size_t posStart = 0;
	int textLine = 30;

	while (posEnd != std::string::npos) {
		std::string str = text.substr(posStart, posEnd - posStart);
		cv::putText(image, str, cv::Point(5, textLine),
			cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
			1.0, // Scale. 2.0 = 2x bigger
			cv::Scalar(255, 255, 255), // Color
			1); // Thickness
		posStart = posEnd + 1;
		posEnd = text.find("\n", posEnd + 1);
		textLine += 20;
	}
}

int main(int argc, const char ** argv)
{
	//suppress c4127 warning
	TIXMLASSERT(true);

	//test gps file
	char* fileName = "data\\test.gpx";
	ifstream fp(fileName);
	if (!fp.is_open()) {
		cout << "Cannot open data\test.gpx" << endl;
		exit(1);
	}
	fp.close();

	GPX gpx(fileName);
	GpxRecord record = gpx.getRecord();

	cv::Mat image;
	cv::Mat grayImage;

	cv::Mat procImage;	cv::VideoCapture capture("data\\test.avi");
	assert(capture.isOpened() && "Input file could not be opened");
	assert(capture.read(image) && "Image not readable from file");
	cv::cvtColor(image, grayImage, CV_RGB2GRAY);
	cv::resize(grayImage, procImage, grayImage.size() / 2);
	int imW = procImage.cols;
	int imH = procImage.rows;

	cv::VideoWriter outVideo;
	int fps = 30;
	outVideo.open("data\\result.avi", -1, fps, procImage.size(), 0);
	assert(outVideo.isOpened());

	//height, tilt, pan, xfov, yfov
	Camera camera(cameraHeight, cameraTilt, cameraPan, cameraXFov, cameraYFov);
	double pixelDensity = camera.getPixelsDensity(imW);
	cout << "Pixel Density = " << pixelDensity << endl;

	cv::namedWindow("Video");
	char key = 'x';
	int frameNumber = 1;
	int gpsCount = 0;
	while (capture.read(image) && key != 'q') {

		if (++gpsCount % 30 == 0 && record.timeStamp != "") {
			record.printOut();
			record = gpx.getNextRecord();
		}

		cv::cvtColor(image, grayImage, CV_RGB2GRAY);
		cv::resize(grayImage, procImage, grayImage.size() / 2);

		double roadHeading = record.heading;

		int cameraHeadingDiff = 28;
		int numLanes = 2;
		int currentLane = 0;
		if (frameNumber >= 940 && frameNumber <= 2050) {
			currentLane = 1;
			cameraHeadingDiff = 30;
		}
		
		cv::Point vPoint = camera.getVanishingPoint(imW, imH, cameraHeadingDiff);

		cv::Point *point = new cv::Point[numLanes + 1]; //number of lanes
		double xShift;
		double lineX;
		
		//now loop each lane:
		//for (currentLane = 0; currentLane < numLanes; ++currentLane) {

			//project all lanes
			for (int i = 0; i < numLanes + 1; i++) {

				if (i <= currentLane) { //left of vehicle
					xShift = (currentLane - i) * laneWidth + laneWidth / 2.0 + cameraShift;
					lineX = imW / 2.0 - xShift * pixelDensity * camera.L / sqrt(camera.L * camera.L + xShift * xShift);
				}
				else {
					xShift = (i - currentLane - 1) * laneWidth + laneWidth / 2.0 - cameraShift;
					lineX = imW / 2.0 + xShift * pixelDensity * camera.L / sqrt(camera.L * camera.L + xShift * xShift);
				}

				cv::line(procImage, vPoint, cv::Point(static_cast<int>(lineX), imH - 1), cv::Scalar(255, 0, 0), 3);
			}
		//}

		std::stringstream stream;
		stream << std::fixed << std::setprecision(2) << roadHeading;
		std::string text = "Frame No.: " + std::to_string(frameNumber) + "\n"
			+ "Road heading: " + stream.str() + "\n"
			+ "Number of lanes: " + std::to_string(numLanes) + "\n"
			+ "Currant lane: " + std::to_string(currentLane) + "\n";
		putTextOnImage(text, procImage);

		outVideo.write(procImage);
		cv::imshow("Video", procImage);
		key = cv::waitKey(33);
		++frameNumber;
	}



	outVideo.release();
	capture.release();

	return 0;
}
