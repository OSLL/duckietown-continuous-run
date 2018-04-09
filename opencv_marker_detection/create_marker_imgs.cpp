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


void createArucoMarkers()
{
	Mat outputMarker;

	/* Choosing 6x6 size markers from aruco Dictionary, quantity: 50 */
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_6X6_50);

	for (int i = 0; i<200; i++)
	{
		/*drawing marker from the dictionary, 500x500 px */
		aruco::drawMarker(markerDictionary, i, 500, outputMarker, 1);

		ostringstream convert;
		
		/*giving markers their names and put new images into project folder*/
		string imageName = "6X6Marker_";
		convert << imageName << i << ".jpg";
		imwrite(convert.str(), outputMarker);

	}
}

int main(int argv, char** argc) {
	createArucoMarkers();
}