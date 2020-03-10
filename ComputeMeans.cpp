//
// Created by Pitam Mitra on 2019-10-24.
//


#include "CoreImageOps.hpp"
#include <climits>


std::vector<cv::Mat> SKCCDImage::MakeMeanImgOfFrames(cv::Mat &skImage, int StartFrame, int EndFrame){

    /*Mean and stdev frame name */
    int NFrameDiff = EndFrame-StartFrame;

    /*Is EndFrame > NDCM? If so, take the last frame as EndFrame*/
    if (EndFrame > this->nChargeMeasurements){
        std::cout<<"Warning: EndFrame is greater than NDCMs. Re-assigning EndFrame to the last charge measurement.\n";
        EndFrame = this->nChargeMeasurements;
    }

    /*CV Std images*/
    cv::Mat _MeanImage = cv::Mat(this->ImageYMax, this->ImageXMax, CV_64F);
    cv::Mat _StdevImage = cv::Mat(this->ImageYMax, this->ImageXMax, CV_64F);

    /*Temporary variables we will need*/
    cv::Scalar _ThisNDCMMean, _ThisNDCMStd;
    int SkipperX_Start, SkipperY_Start;

    for (int j = 0; j < this->ImageYMax ; j++) {
        for (int i = 0; i < this->ImageXMax; i++) {
            /*For a single NDCM read sequence, these are the start and end values of the pixel coordinates*/
            SkipperX_Start = this->skImgX(StartFrame, i);
            SkipperY_Start = this->skImgY(StartFrame, j);

            /*Region of Interest - 1 NDCM sequence with a height of 1 pixel*/
            cv::Rect _thisSkSeqROI = cv::Rect(SkipperX_Start, SkipperY_Start, EndFrame, 1);

            /*Mean and stdev of this sequence*/
            cv::Mat _thisSkSequence(this->_cvSkImage, _thisSkSeqROI);
            cv::meanStdDev(_thisSkSequence, _ThisNDCMMean, _ThisNDCMStd);

            //if (InvertScale) _MeanImage.at<double>(j, i) = (double) USHRT_MAX - _ThisNDCMMean[0];
            //else _MeanImage.at<double>(j, i) = _ThisNDCMMean[0];

            _MeanImage.at<double>(j, i) = 65535.0 - _ThisNDCMMean[0]; //65535.0 = max value of unsigned short
            _StdevImage.at<double>(j, i) = _ThisNDCMStd[0];

        }
    }

    std::vector<cv::Mat> MeanStdImages{_MeanImage, _StdevImage};
    return MeanStdImages;

}

/*On some settings, the image show up with a baseline that grows with the row number.
 *These are not necessarily bad settings - you get no charge loss and the single electron
 *response with these settings.
 */

cv::Mat SKCCDImage::YBaselineCorrect(cv::Mat &inImage){

    int XMax = inImage.size().width;
    int YMax = inImage.size().height;


    /*CV Std images*/
    cv::Mat _MeanImageBlFx = cv::Mat(YMax, XMax, CV_64F);


    double _thisRowBaseline;
    double _baselineCorrectedValue;

    std::vector<double> colValue;
    std::vector<double> colOffset;

    for (int j = 0; j < YMax ; j++) {

        cv::Rect _thisRowDef = cv::Rect(0,j,XMax,1);
        cv::Mat _thisRow = inImage(_thisRowDef);

        _thisRowBaseline = fMedianPitam(_thisRow);


        for (int i = 0; i < XMax; i++) {
            double _baselineFixed = _thisRow.at<double>(0,i)-_thisRowBaseline;
            _MeanImageBlFx.at<double>(j,i) = _baselineFixed;
        }
    }

   

    return _MeanImageBlFx;
}