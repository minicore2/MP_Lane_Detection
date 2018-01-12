#pragma once

#include <string>
#include <opencv2\opencv.hpp>
#include <cassert>

const double PI = 4.0 * atan(1.0);
const double toDegree = 180.0 / PI;
const double toRadius = PI / 180.0;
const double laneWidth = 3.5; //meter
const double cameraShift = 1.8; //meter
const double cameraHeight = 4.0;
const double cameraTilt = 11.5;
const double cameraPan = 20;
const double cameraXFov = 55;
const double cameraYFov = 42;

class Camera {
private:
	double height;
	double tiltRad;
	double xFovRad;
	double yFovRad;
	double panRad;

	cv::Point vPoint;
public:
	Camera(double h, double t, double pan, double xfov, double yfov) :
		height(h),
		tiltRad(t * toRadius),
		xFovRad(xfov * toRadius),
		yFovRad(yfov * toRadius),
		panRad(panRad * toRadius) {
		assert(abs(yFovRad) > 0);
		L = height / tan(yFovRad / 2);
		W = 2 * L * tan(xFovRad / 2);
	};

	cv::Point getVanishingPoint(int imWidth, int imHeight, double pan) {
		cv::Point p;
		p.y = static_cast<int>(imHeight * (1.0 - tan(tiltRad) / tan(yFovRad / 2.0)));
		p.x = static_cast<int>(imWidth / 2 + (imHeight - p.y) * tan(-pan * toRadius));
		return p;
	}

	double getPixelsDensity(int imWidth) {
		return imWidth * tan(yFovRad/2 + tiltRad) / (2 * height * tan(xFovRad / 2));
	}

	double W; //fov width
	double L; //fov y starting point
};
