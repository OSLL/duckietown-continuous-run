#include "opencv2\core.hpp"
#include "opencv2\imgcodecs.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\aruco.hpp"
#include "opencv2\calib3d.hpp"

#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

const float arucoSquareDimension = 0.0064f;

int startWebcamMonitoring(const Mat& cameraMatrix, const Mat& distanceCoefficients, float arucoSquareDimension)
{
	Mat frame;

	std::vector<int> markerIds;
	std::vector<std::vector<Point2f>> markerCorners, rejectedCandidates;
	aruco::DetectorParameters parameters;

	/*loading used dictionary*/
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_6X6_50);

	/*openning the webcam stream*/
	VideoCapture vid(0);

	if (!vid.isOpened())
	{
		return -1;
	}

	namedWindow("Webcam", CV_WINDOW_AUTOSIZE);

	std::vector<Vec3d> rotationVectors, translationVectors;

	while (true)
	{
		if (!vid.read(frame))
		{
			break;
		}

		/*finding markers*/
		aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIds);
		/*configuring the position of markers*/
		aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDimension, cameraMatrix, distanceCoefficients, rotationVectors, translationVectors);

		for (int i = 0; i<markerIds.size(); i++)
		{
			/*drawing coordinate system for every detected markers*/
			aruco::drawAxis(frame, cameraMatrix, distanceCoefficients, rotationVectors[i], translationVectors[i], 0.01f);
		}
		imshow("Webcam", frame);
		if (waitKey(30) >= 0)
		{
			break;
		}
	}

	return 1;
}

bool loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients)
{
	/*loading parameters in the way, they were written*/
	ifstream inStream(name);
	if (inStream)
	{
		uint16_t rows;
		uint16_t colums;

		inStream >> rows;
		inStream >> colums;

		cameraMatrix = Mat(Size(colums, rows), CV_64F);

		for (int r = 0; r<rows; r++)
		{
			for (int c = 0; c < colums; c++)
			{
				double read = 0.0f;
				inStream >> read;
				cameraMatrix.at<double>(r, c) = read;
				cout << cameraMatrix.at<double>(r, c) << "\n";
			}
		}

		inStream >> rows;
		inStream >> colums;

		distanceCoefficients = Mat::zeros(rows, colums, CV_64F);

		for (int r = 0; r<rows; r++)
		{
			for (int c = 0; c < colums; c++)
			{
				double read = 0.0f;
				inStream >> read;
				distanceCoefficients.at<double>(r, c) = read;
				cout << distanceCoefficients.at<double>(r, c) << "\n";

			}
		}
		inStream.close();
		return true;
	}
	return false;
}

int main(int argv, char** argc) {
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	Mat distanceCoefficients;

	/*loading previously created camera calibration file*/
	loadCameraCalibration("calibro",cameraMatrix,distanceCoefficients);
	/*using the calibration parameters, finding markers*/
	startWebcamMonitoring(cameraMatrix, distanceCoefficients, arucoSquareDimension);
}