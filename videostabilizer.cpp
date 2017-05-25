#include "videostabilizer.h"


MotionModel motionModel(const string &str){
    if (str == "transl") return MM_TRANSLATION;
    if (str == "transl_and_scale") return MM_TRANSLATION_AND_SCALE;
    if (str == "rigid") return MM_RIGID;
    if (str == "similarity") return MM_SIMILARITY;
    if (str == "affine") return MM_AFFINE;
    if (str == "homography") return MM_HOMOGRAPHY;
    return MM_TRANSLATION;
}

VideoStabilizer::VideoStabilizer()
{}

VideoStabilizer::~VideoStabilizer(){}

void VideoStabilizer::stabilize(){
    if (inputPath.empty() || outputPath.empty())
        return;
    start();
}

void VideoStabilizer::start(){
    StabilizerBase *stabilizer = nullptr;
    Ptr<VideoFileSource> source = makePtr<VideoFileSource>(inputPath);

    TwoPassStabilizer *twoPassStabilizer = new TwoPassStabilizer();
    stabilizer = twoPassStabilizer;
    twoPassStabilizer->setEstimateTrimRatio(true);
    twoPassStabilizer->setMotionStabilizer(makePtr<GaussianMotionFilter>(radius));
    stabilizer->setFrameSource(source);

    MotionModel motModel = motionModel(model);
    Ptr<MotionEstimatorRansacL2> est = makePtr<MotionEstimatorRansacL2>(motModel);
    RansacParams ransac = est->ransacParams();
    ransac.eps = outlierRatio;
    est->setRansacParams(ransac);
    est->setMinInlierRatio(minInlierRatio);
    Ptr<IOutlierRejector> outlierRejector = makePtr<NullOutlierRejector>();
    if (isLocalOutlierRejection) {
        Ptr<TranslationBasedLocalOutlierRejector> tblor = makePtr<TranslationBasedLocalOutlierRejector>();
        RansacParams ransacParams = tblor->ransacParams();
        tblor->setRansacParams(ransacParams);
        outlierRejector = tblor;
    }
    Ptr<KeypointBasedMotionEstimator> kbest = makePtr<KeypointBasedMotionEstimator>(est);
    kbest->setDetector(GFTTDetector::create(NKPS));
    kbest->setOutlierRejector(outlierRejector);

    stabilizer->setMotionEstimator(kbest);

    stabilizer->setRadius(radius);

    stabilizer->setTrimRatio(trimRatio);
    stabilizer->setCorrectionForInclusion(inclConstr);

    if (borderMode == "reflect") stabilizer->setBorderMode(BORDER_REFLECT);
    else if (borderMode == "replicate")	stabilizer->setBorderMode(BORDER_REPLICATE);
    else if (borderMode == "const")	stabilizer->setBorderMode(BORDER_CONSTANT);
    else stabilizer->setBorderMode(BORDER_CONSTANT);

    stabilize(dynamic_cast<IFrameSource*>(stabilizer), source->fps());
    delete stabilizer;
}

void VideoStabilizer::stabilize(IFrameSource *stabilizedFrames, double outputFps){
    VideoWriter writer;
    Mat stabilizedFrame;
        // for each stabilized frame
    while (!(stabilizedFrame = stabilizedFrames->nextFrame()).empty()){
        if (!writer.isOpened())
            writer.open(outputPath, VideoWriter::fourcc('X','V','I','D'),
                        outputFps, stabilizedFrame.size());
            writer << stabilizedFrame;

        if (imShow) {		// show stabilized frame
            imshow("stabilizedFrame", stabilizedFrame);
            char key = static_cast<char>(waitKey(3));
            if (key == 27) { cout << endl; break; }
        }
    }
}

