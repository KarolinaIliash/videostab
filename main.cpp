#include "videostabilizer.h"

int main(){
    VideoStabilizer videostab;
    videostab.inputPath = "D:\\QTProjects\\opencvtry\\vid.mp4";
    videostab.outputPath = "D:\\QTProjects\\opencvtry\\stabvid.avi";
    videostab.imShow = true;
    videostab.stabilize();
}
