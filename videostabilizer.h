#ifndef VIDEOSTABILIZER_H
#define VIDEOSTABILIZER_H

#include <iostream>
#include <fstream>


#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/videostab.hpp>

using namespace std;
using namespace cv;
using namespace cv::videostab;

struct VideoStabilizer{

    VideoStabilizer();
    ~VideoStabilizer();

    bool imShow	= false;
    string inputPath;
    string outputPath;
    string model = "affine";
    float outlierRatio = 0.5f;
    float minInlierRatio = 0.1f;
    bool isLocalOutlierRejection = false;
    int	NKPS = 1000;
    int	radius = 15;
    float trimRatio	= 0.1f;
    bool inclConstr = false;
    string borderMode = "replicate";

    void stabilize();

private:
    void start();
    void stabilize(IFrameSource *stabilizedFrames, double outputFps);
};

#endif // VIDEOSTABILIZER_H
