#include <opencv2/opencv.hpp>
#include "CoreImageOps.hpp"


int main (int argc, char** argv) {


    /*If we dont have all the inputs in place, then show the help text*/
    if (argc < 4){
        std::cout<<"DAMICM Skipper CCD Reconstruction code.\n"
                   "This code converts a skipper CCD image into: \n1. Mean of all NDCMs.\n2.Stdev of all NDCMs.\n"
                   "Input: skCCD Image (fits file), output file prefix, StartFrame, EndFrame \n"
                   "Output: 1. Mean of NDCMs: Avg_<output file name>_<startframe>_to_<endframe>.fits\n"
                   "Output: 2. Stdev of NDCMs: Stdev_<output file name>_<startframe>_to_<endframe>.fits\n"
                   "Example: ./DAMICMRecon Image_07.fits ReconImage07 2 15\n";

        return 1;
    }


    /*The required inputs*/
    std::string InputFileName = std::string(argv[1]);
    std::string OutputPrefixName = std::string(argv[2]);
    int StartFrame = atoi(argv[3]);
    int EndFrame = atoi(argv[4]);

    /*Load image and compute mean, stdev*/
    std::vector<cv::Mat> MeanStdImage;
    SKCCDImage ThisCCDImage(InputFileName);
    ThisCCDImage.LoadFits();

    if (ThisCCDImage.nChargeMeasurements == 1){
        MeanStdImage.push_back(ThisCCDImage._cvSkImage);
        MeanStdImage.push_back(ThisCCDImage._cvSkImage);
    } else {
        MeanStdImage = ThisCCDImage.MakeMeanImgOfFrames(ThisCCDImage._cvSkImage, StartFrame, EndFrame);
    }

    std::string OutFileName = OutputPrefixName+"_"+std::to_string(StartFrame)+"_to_"+std::to_string(EndFrame)+".fits";

    std::string MeanOutFileName = "Avg_"+OutputPrefixName;
    std::string StdevOutFileName = "Stdev_"+OutputPrefixName;

    ThisCCDImage.SaveFitsSK(MeanStdImage[0],MeanOutFileName);
    ThisCCDImage.SaveFitsSK(MeanStdImage[1],StdevOutFileName);

    return 0;

}
