//
// Created by Pitam Mitra on 2019-02-18.
//

#ifndef CCDDRONE_FITSOCV_HPP
#define CCDDRONE_FITSOCV_HPP


#include "opencv2/opencv.hpp"
#include <string>


double fMedianPitam(cv::Mat SingleSkipROI);


class SKCCDImage {

private:

    /*Image file names*/
    std::string inFitsFileName;
    std::string outFitsFileName;


    /*Image parameters*/
    long xaxismax;
    long yaxismax;

    int xAxisOffset;
    std::string Amplifier;


public:
    SKCCDImage(std::string );
    ~SKCCDImage();

    /*Loading and saving images*/
    void LoadFits( );
    void SaveFitsSK(cv::Mat, std::string, int );



    /*CCD Image offsets*/
    int CCDImageOffset;
    int nIgnoreFirstNDCMs;

    /*NDCMs*/
    short nChargeMeasurements;

    /*Indexers*/
    int skImgX(int , int );
    int skImgY(int , int );

    /*SkImageParameters*/
    long ImageXMax;
    long ImageYMax;

    std::string ImageFileStem;

    /*The OpenCV image container*/
    cv::Mat _cvSkImage;
    //cv::Mat _cvSkMergedImage;
    double *_cvSkImageData;

    /*Image Ops routines*/
    std::vector<cv::Mat> MakeMeanImgOfFrames(cv::Mat &skImage, int StartFrame, int EndFrame);
    cv::Mat YBaselineCorrect(cv::Mat &inImage);


};


#endif //CCDDRONE_FITSOCV_HPP
