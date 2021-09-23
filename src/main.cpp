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

Mat process(Mat& image, Net& nnet, int W_in, int H_in);
void findBodyPartPositions(vector<Point>& locations, Mat& nnoutput, 
		const int imageW, const int imageH, const int outputW, const int outputH, float thresh);
void drawSkeleton(const vector<Point>& locations, Mat& image);
void addFrameInfo(Mat& image, Net& nnet);

int main(int argc, char** argv)
{
	/* Parse command line arguments */
	/* Important: use spaces to separate out the columns, NOT tabs */
	const String keys =
		"{path p        |<none>                 | path to input file      }"
		"{pathout o     |<none>                 | output file for image/video      }"
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

	/* Choose the camera if the inputFile is empty */
	bool retVal = (!inputFile.empty()) ? vidcap.open(inputFile) : vidcap.open(0);
	
	/* Error checking - say if no camera exists */
	if (!retVal)
	{
		cerr << "ERROR: Can't open image/video." << endl;
		exit(-1);
	}

	/* Do different things depending on whether the video is selected or not */
	if (!parser.get<bool>("video"))
	{
		cout << "IMAGE" << endl;
		Mat image;
		vidcap.read(image);
	
		/* Store height and width of image frame */
		const int imageH = image.size[0];
		const int imageW = image.size[1];
		
		/* Send through network, obtaining a 4-dim Mat object, an array of heatmaps */	
		Mat nnoutput = process(image, nnet, W_in, H_in);

		const int outputH = nnoutput.size[2];
		const int outputW = nnoutput.size[3];

		/* Find the position of each body part */
		vector<Point> locations;
		findBodyPartPositions(locations, nnoutput, imageW, imageH, outputW, outputH, thresh);

		/* Draw the skeleton lines for each of the posePairs */
		drawSkeleton(locations, image);
	
		/* Frame information for image */
		addFrameInfo(image, nnet);
		
		/* Write the image to file and display it on the screen. */
		imwrite(outputFile, image);
		cout << "Output written to " << outputFile << endl;

		imshow("Pose Estimation using OpenCV", image);

		/* Wait for the user to exit the program */
		waitKey();
	}
	else
	{	
		cout << "VIDEO" << endl;
	
		/* Use the frame rate from the original video for the output video */	
		double fps = vidcap.get(CAP_PROP_FPS);

		/* Construct a VideoWriter object in order to write frames to it */
		VideoWriter video(outputFile, VideoWriter::fourcc('M','J','P','G'), fps, Size(W_in,H_in));

		/* Start grabbing images from video camera */
		while (waitKey(1) < 0)
		{
			Mat image;
			vidcap.read(image);
			if (image.empty())
			{
				/* Indicate to the user that the writing to the output has been completed */
				cout << "Output written to " << outputFile << endl;
				/* Will loop to here if an image was passed in instead of a video */
				waitKey();
				//cerr << "ERROR: blank frame grabbed" << endl;
				break;
			}
	
			/* Store height and width of image frame */
			const int imageH = image.size[0];
			const int imageW = image.size[1];

			/* Send through network, obtaining a 4-dim Mat object, an array of heatmaps */	
			Mat nnoutput = process(image, nnet, W_in, H_in);

			const int outputH = nnoutput.size[2];
			const int outputW = nnoutput.size[3];

			/* Find the position of each body part */
			vector<Point> locations;
			findBodyPartPositions(locations, nnoutput, imageW, imageH, outputW, outputH, thresh);

			/* Draw the skeleton lines for each of the posePairs */
			drawSkeleton(locations, image);
	
			/* Frame information for image */
			addFrameInfo(image, nnet);

			/* Write the image to the video */
			video.write(image);

			/* See the output frames as they're being written */
			imshow("Pose Estimation using OpenCV", image);
		}
	}	

	return 0;
}

Mat process(Mat& image, Net& nnet, int W_in, int H_in)
{
	/* Send through network */
	Mat nninput = blobFromImage(image, 1.0, Size(W_in, H_in), Scalar(0,0,0), false, false);
	nnet.setInput(nninput);

	/* Obtain a 4-dim Mat object, an array of heatmaps */	
	return nnet.forward();	
}

void findBodyPartPositions(vector<Point>& locations, Mat& nnoutput, 
		const int imageW, const int imageH, const int outputW, const int outputH, float thresh)
{
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

		/* Add this point or a proxy for None depending on threshold */ 
		(maxVal > thresh) ? locations.push_back(Point(W,H)) : locations.push_back(Point(-1,-1));
	}
}

void drawSkeleton(const vector<Point>& locations, Mat& image)
{
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
}

void addFrameInfo(Mat& image, Net& nnet)
{
	vector<double> timings;
	int64 t = nnet.getPerfProfile(timings); 
	double freq = getTickFrequency() / 1000;
	string text = to_string(t/freq) + " ms";
	putText(image, text , Point(10,20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0));
}
