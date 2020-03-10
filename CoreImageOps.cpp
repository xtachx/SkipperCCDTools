//
// Created by Pitam Mitra on 2019-02-18.
//


#include "opencv2/opencv.hpp"
#include <string>
#include <algorithm>
#include "CoreImageOps.hpp"
#include <experimental/filesystem>


SKCCDImage::SKCCDImage(std::string inFile) {

    /*Assignments*/
    this->inFitsFileName = inFile;

    /*This is the number of reads to ignore in the begining of each row. 
     *The first 2 pixels are always ignored because they dont represent 
     *anything coming out of the CCD. 
     */

    this->xAxisOffset = 2;          

    this->ImageFileStem = std::experimental::filesystem::path(inFile).stem();


}

SKCCDImage::~SKCCDImage() {


}


/*Skipper image's indexing */
int SKCCDImage::skImgX(int frame, int X) {

    /*Simple case, where there is only one amplifier working*/
    if (this->Amplifier == "L" || this->Amplifier == "U") {
        return this->xAxisOffset+(this->nChargeMeasurements*X+frame);

    /*Now the not so simple case of two amplifiers - one on each end*/
    } else {
        int halfway_point = (this->xaxismax/2)-this->xAxisOffset;
        int half_x_axis = halfway_point / this->nChargeMeasurements;
        /*From the left*/
        if (X<half_x_axis) return this->xAxisOffset+(this->nChargeMeasurements*X+frame);
        /*From the right*/
        else return this->xaxismax-(this->xAxisOffset+this->nChargeMeasurements*X+frame);
    }
}

int SKCCDImage::skImgY(int frame, int Y) {
    return Y;
}



/*Fast Median calculations*/
double fMedianPitam(cv::Mat SingleSkipROI){
    int _MatSize = SingleSkipROI.cols;
    std::vector<double> _tempMedianCalc(SingleSkipROI.begin<double>(),SingleSkipROI.end<double>()); //(SingleSkipROI.begin<double>(), SingleSkipROI.end<double>());
    std::nth_element(_tempMedianCalc.begin(), _tempMedianCalc.begin()+_tempMedianCalc.size()/2, _tempMedianCalc.end());

    return _tempMedianCalc[_tempMedianCalc.size()/2];
}