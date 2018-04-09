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

const float calibrationSquareDimension = 0.0218f;  //Size of one square on printed out chessboard picture,  in meters
const float arucoSquareDimension = 0.0064f; // Size of printed out marker, in meters
const Size chessboardDimensions = Size(6, 9); //The board size the programm is looking for


/*creating the vector with ideal square corners position*/
void createKnownBoardPosition(Size boardSize, float squareEdgeLength, vector<Point3f>& corners)
{
	for (int i = 0; i < boardSize.height; i++)
	{
		for (int j = 0; j < boardSize.width; j++)
		{
			corners.push_back(Point3f(j* squareEdgeLength, i * squareEdgeLength, 0));
		}
	}
}

/*a function, which uses the vector of gathered images with board, to detect the chessboard itself and draw lines around detected corners */
void getChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& allFoundCorners, bool showResults = false)
{
	for (vector<Mat>::iterator iter = images.begin(); iter != images.end(); iter++)
	{
		vector<Point2f> pointBuf;
		/*take one image and tries to find a chessboard*/
		bool found = findChessboardCorners(*iter, Size(9, 6), pointBuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);

		if (found)
		{
			/*if the board was detected, adds found points to the result vector*/
			allFoundCorners.push_back(pointBuf);
		}

		/*additional info about capturing, switched off by default*/
		if (showResults)
		{
			drawChessboardCorners(*iter, Size(9, 6), pointBuf, found);
			imshow("Looking for corners", *iter);
			waitKey(0);
		}
	}
}

/*calibrates the camera, using gathered information from a succesfully detected pictures*/
void cameraCalibration(std::vector<Mat> calibrationImages, Size boardSize, float squareEdgeLength, Mat& cameraMatrix, Mat& distanceCoefficients)
{
	vector<vector<Point2f>> checkerboardImageSpacePoints;
	getChessboardCorners(calibrationImages, checkerboardImageSpacePoints, false);

	vector<vector<Point3f>> worldSpaceCornerPoints(1);
	createKnownBoardPosition(boardSize, squareEdgeLength, worldSpaceCornerPoints[0]);
	worldSpaceCornerPoints.resize(checkerboardImageSpacePoints.size(), worldSpaceCornerPoints[0]);

	vector<Mat> rVectors, tVectors;
	distanceCoefficients = Mat::zeros(8, 1, CV_64F);

	calibrateCamera(worldSpaceCornerPoints, checkerboardImageSpacePoints, boardSize, cameraMatrix, distanceCoefficients, rVectors, tVectors);
}


/*write down to a file a result calibration parametrs for a future use*/
bool saveCameraCalibration(string name, Mat cameraMatrix, Mat distanceCoefficients)
{
	ofstream outStream(name);
	if (outStream)
	{
		uint16_t rows = cameraMatrix.rows;
		uint16_t colums = cameraMatrix.cols;

		outStream << rows << endl;
		outStream << colums << endl;

		for (int r = 0; r< rows; r++)
		{
			for (int c = 0; c< colums; c++)
			{
				double value = cameraMatrix.at<double>(r, c);
				outStream << value << endl;
			}
		}

		rows = distanceCoefficients.rows;
		colums = distanceCoefficients.cols;

		outStream << rows << endl;
		outStream << colums << endl;

		for (int r = 0; r< rows; r++)
		{
			for (int c = 0; c< colums; c++)
			{
				double value = distanceCoefficients.at<double>(r, c);
				outStream << value << endl;
			}
		}

		outStream.close();
		return true;
	}
	return false;
}


int main(int argv, char** argc)
{
	Mat cameraMatrix = Mat::eye(3,3,CV_64F);
	Mat distanceCoefficients;
	
	Mat frame;
	Mat drawToFrame;

	vector<Mat> savedImages;

	vector<vector<Point2f>> markerCorners, rejectedCandidates;

	/*connect to web camera*/
	VideoCapture vid(0);

	if (!vid.isOpened())
	{
		return 0;
	}

	/*within succesfull connection creates a window for a camera*/
	int framesPerSecond = 20;
	namedWindow("Webcam", CV_WINDOW_AUTOSIZE);

	while (true)
	{
		if (!vid.read(frame))
		{
			break;
		}

		vector<Vec2f> foundPoints;
		bool found = false;


		/*finds corners and draws lines over the main frame with webcam video*/
		found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		frame.copyTo(drawToFrame);
		drawChessboardCorners(drawToFrame, chessboardDimensions, foundPoints, found);
		if (found)
		{
			imshow("webcam", drawToFrame);
		}
		else imshow("webcam", frame);
		char charachter = waitKey(100 / framesPerSecond);

		/*command keys:*/
		switch (charachter)
		{
			/*Space - for taking a picture*/
		case ' ':
			if (found)
			{
				Mat temp;
				frame.copyTo(temp);
				savedImages.push_back(temp);
			}
			break;
			/*Enter - for starting the process of calibration(only on the condition, that not less than 15 images was taken*/
		case 13:
			if (savedImages.size()>15)
			{
				cameraCalibration(savedImages, chessboardDimensions, calibrationSquareDimension, cameraMatrix, distanceCoefficients);
				saveCameraCalibration("filename", cameraMatrix, distanceCoefficients);
			}
			break;
			/*Esc - is for the determination of the algorithm*/
		case 27:
			return 1;
			break;
		}
	}
	return 0;
	
}