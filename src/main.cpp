#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
using namespace cv;
using namespace cv::dnn;

#include <iostream>
using namespace std;

/* include set up header files for body parts and pose pairs */
#include "headers/bodyparts.h"
#include "headers/posepairs.h"
#include "headers/colourmap.h"

int main(int argc, char** argv)
{
	/* Parse command line arguments */
	/* Important: use spaces to separate out the columns, NOT tabs */
	const String keys =
		"{path p        |<none>                 | path to input file      }"
		"{pathout o     |./outimages/out.jpg    | output file for image/video      }"
		"{threshold t   |0.2                    | threshold value for heatmap }"
		"{width w       |368                    | preprocess input image width }"
		"{height u      |368                    | preprocess input image height }"
		"{video v       |                       | flag to indicate video }"
		"{help h        |                       | print this help message }"
		;

	CommandLineParser parser(argc, argv, keys);
	String inputFile = parser.get<String>("path");
	String outputFile = parser.get<String>("pathout");
	float thresh = parser.get<float>("threshold");
	int W_in = parser.get<int>("width");
	int H_in = parser.get<int>("height");
	
	/* Check for help */
	if (parser.get<bool>("help")) 
	{
		cout << "A sample app to demonstrate single human pose detection"
			<< " with a pretrained OpenPose Deep Neural Network." << endl;
		parser.printMessage();
		return 0;
	}
	
	/* Read in pre-trained NN" */
	Net nnet = readNetFromTensorflow("./model/graph_opt.pb");
	
	/* Construct VideoCapture object */
	VideoCapture vidcap;
	bool retVal = (!inputFile.empty()) ? vidcap.open(inputFile) : vidcap.open(0);

	if (!retVal)
	{
		cerr << "ERROR: Can't open image/video." << endl;
		exit(-1);
	}

	//TEST -- TODO
	/* TODO - FIX THE DIMENSIONS of the rhs to be the same as the input size video 
	 * Fix the output string to be variable */
	//VideoWriter video("ellieout.mov", VideoWriter::fourcc('M','J','P','G'), 10, Size(360,640));
	//VideoWriter video("ellieout2.mov", VideoWriter::fourcc('M','J','P','G'), 10, Size(364,640));

	/* Start grabbing images from video camera */
	while (waitKey(1) < 0)
	{
		Mat image;
		vidcap.read(image);
		if (image.empty())
		{
			/* Will loop to here if an image was passed in instead of a video */
			waitKey();
			//cerr << "ERROR: blank frame grabbed" << endl;
			break;
		}
	
		/* Store height and width of image frame */
		const int imageH = image.size[0];
		const int imageW = image.size[1];
		
		/* Send through network */
		Mat nninput = blobFromImage(image, 1.0, Size(W_in, H_in), Scalar(0,0,0), false, false);
		nnet.setInput(nninput);

		/* Obtain a 4-dim Mat object, an array of heatmaps */	
		Mat nnoutput = nnet.forward();	

		const int outputH = nnoutput.size[2];
		const int outputW = nnoutput.size[3];

		/* Find the position of each body part */
		vector<Point> locations;
		const int numBodyParts = bodyParts.size();
		for (int i = 0; i < numBodyParts; ++i)
		{
			/* Heatmap slice */
			Mat heatMap(outputH, outputW, CV_32F, nnoutput.ptr(0,i));
			
			/* Find global max value and location */
			double maxVal;	
			Point maxLoc;
			minMaxLoc(heatMap, nullptr, &maxVal, nullptr, &maxLoc);
			
			/* Scale to image size */
			int W = (imageW * maxLoc.x) / outputW;
			int H = (imageH * maxLoc.y) / outputH;

			/* Add this point or a proxy for None depending on threshold */ //TODO - can we do better?
			(maxVal > thresh) ? locations.push_back(Point(W,H)) : locations.push_back(Point(-1,-1));

		}

		/* Draw the skeleton lines for each of the posePairs */
		for (auto& posepr : posePairs)
		{
			/* Find body part indices into locations vector */
			const int idxFrom = bodyParts.at(posepr.first.first);
			const int idxTo = bodyParts.at(posepr.first.second);
			
			/* Only draw for body parts found */
			if(locations[idxFrom].x > 0 &&
				locations[idxFrom].y > 0 &&
				locations[idxTo].x > 0 &&
				locations[idxTo].y > 0)
			{
				String colName = posepr.second;
				Colour col = colourMap.at(colName);	
				line(image, locations[idxFrom], locations[idxTo], Scalar(col.B, col.G, col.R), 5);
				ellipse(image, locations[idxFrom], Size(5,5), 0, 0, 360, Scalar(col.B,col.G,col.R), FILLED);
				ellipse(image, locations[idxTo], Size(5,5), 0, 0, 360, Scalar(col.B,col.G,col.R), FILLED);
			}
		}
	
		/* Frame information for image */
		vector<double> timings;
		int64 t = nnet.getPerfProfile(timings); 
		double freq = getTickFrequency() / 1000;
		string text = to_string(t/freq) + " ms";
		putText(image, text , Point(10,20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0));

		//TEST - TODO
		//video.write(image);
		//imwrite("epcoutput.jpg", image);
		imwrite(outputFile, image);

		imshow("Pose Estimation using OpenCV", image);
	}	

	return 0;
}
