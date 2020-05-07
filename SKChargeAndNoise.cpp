#include <opencv2/opencv.hpp>
#include "CoreImageOps.hpp"
#include "include/CLI11.hpp"
#include "include/ProgressBar.hpp"
#include "fitsio.h"


int main (int argc, char** argv) {


    /*If we dont have all the inputs in place, then show the help text*/
    // if (argc < 4){
    //     std::cout<<"DAMICM Skipper CCD Reconstruction code.\n"
    //                "This code converts a skipper CCD image into: \n1. Mean of all NDCMs.\n2.Stdev of all NDCMs.\n"
    //                "Input: skCCD Image (fits file), output file prefix, StartFrame, EndFrame \n"
    //                "Output: 1. Mean of NDCMs: Avg_<output file name>_<startframe>_to_<endframe>.fits\n"
    //                "Output: 2. Stdev of NDCMs: Stdev_<output file name>_<startframe>_to_<endframe>.fits\n"
    //                "Example: ./DAMICMRecon Image_07.fits ReconImage07 2 15\n";

    //     return 1;
    // }

    //  Command line arguments
    CLI::App damicApp{"DAMICM Skipper CCD Reconstruction code.\n"
                      "This code takes an Skipper image and turns it into a: \n  1. Mean Image.\n  2. Stdev Image.\n"};


    // Input variables
    std::vector<std::string> InputFileNames;
    int StartFrame, EndFrame;
    std::pair<int, int> FrameRange(0, -1); // Default values of the range
    std::string OutputDirectory = std::filesystem::current_path();
    // std::vector<double> FrameRange;

    // Define command line options
    damicApp.add_option("-f, --file", InputFileNames, "Input SKImage filename/list of filenames")
        ->check(CLI::ExistingFile)
        ->required();
    damicApp.add_option("-r, --range", FrameRange, "NDCM Range to perform operation over.");
    damicApp.add_option("-o, --output", OutputDirectory, "Output directory for proecessed file")
        ->check(CLI::ExistingDirectory);


    // Parse argv into arguments
    try {
        damicApp.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return damicApp.exit(e);
    }

    // Create Progress Bar
    std::cout << "Processing files...\n";
    progresscpp::ProgressBar damicReconProgress(InputFileNames.size(), 50);

    // Iterate over all files, processing one at a time
    for(auto file: InputFileNames){

        // std::cout << file << std::endl;

        /*Load image and compute mean, stdev*/
        std::vector<cv::Mat> MeanStdImage;
        SKCCDImage ThisCCDImage(file);
        ThisCCDImage.LoadFits();

        if(ThisCCDImage.nChargeMeasurements == 1) continue;

        // Check the NDCM range is good and process files
        if(FrameRange.first < 0 || FrameRange.first >= ThisCCDImage.nChargeMeasurements) FrameRange.first = 0;
        if(FrameRange.second < 0 || FrameRange.second <= FrameRange.first || FrameRange.second >= ThisCCDImage.nChargeMeasurements)
            FrameRange.second = ThisCCDImage.nChargeMeasurements - 1;
        if (ThisCCDImage.nChargeMeasurements == 1){
            MeanStdImage.push_back(ThisCCDImage._cvSkImage);
            MeanStdImage.push_back(ThisCCDImage._cvSkImage);
        } else {
            MeanStdImage = ThisCCDImage.MakeMeanImgOfFrames(ThisCCDImage._cvSkImage, FrameRange.first, FrameRange.second);
        }

        // Prepare output filenames
        std::string OutputPrefixNameAndType = file.substr(file.find_last_of("/") + 1); // Gets just the filename from the path
        std::string OutputPrefixName = OutputPrefixNameAndType.substr(0, OutputPrefixNameAndType.find_last_of("."));
        std::string OutFileName = OutputDirectory + "/" + OutputPrefixName+"_"+std::to_string(FrameRange.first)+"_to_"+std::to_string(FrameRange.second); // Includes NDCM range in the output filename
        std::string MeanOutFileName = OutFileName + "_Avg.fits";
        std::string StdevOutFileName = OutFileName + "_Stdev.fits";

        // Save the processed files
        ThisCCDImage.SaveFitsSK(MeanStdImage[0], MeanOutFileName, FLOAT_IMG);
        ThisCCDImage.SaveFitsSK(MeanStdImage[1], StdevOutFileName, FLOAT_IMG);

        // Update progress bar
        ++damicReconProgress;
        damicReconProgress.display();

    }

    std::cout << "Processing Complete.\n";
    

    return 0;

}
