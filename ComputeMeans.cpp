//
// Created by Pitam Mitra on 2019-10-24.
//

#include "SKChargeAndNoise.hpp"
#include <climits>


cv::Mat SKChargeAndNoise::MakeMeanImgOfFrames(cv::Mat &skImage, int StartFrame, int EndFrame, std::string CombImageName, bool DrawStdev, bool FrameStore){

    /*Mean and stdev frame name */
    int NFrameDiff = EndFrame-StartFrame;
    EndFrame = EndFrame < this->nChargeMeasurements ? EndFrame : this->nChargeMeasurements-5;

    std::string LongFrameName = CombImageName+"_"+std::to_string(StartFrame)+"_to_"+std::to_string(EndFrame)+"_D"
                                +std::to_string(NFrameDiff);

    //std::string ShortFrameName = LongMeanFrameName;
    std::string ShortFrameName = CombImageName+std::to_string(NFrameDiff);

    //Mean and Stdiv names
    std::string LongMeanFrameName = "Avg_"+LongFrameName;
    std::string ShortMeanFrameName = "Avg_"+ShortFrameName;

    std::string LongStdFrameName = "Stdev_"+LongFrameName;
    std::string ShortStdFrameName = "Stdev_"+ShortFrameName;

    /*ROOT Standard images*/
    int rows = this->ImageYMax;
    int cols = this->ImageXMax;
    TH2D ROOTMeanImg(ShortMeanFrameName.c_str(), LongMeanFrameName.c_str(), cols, 0, cols, rows, 0, rows);
    TH2D ROOTStdImg(ShortStdFrameName.c_str(), LongStdFrameName.c_str() , cols, 0, cols, rows, 0, rows);


    /*CV Std images*/
    cv::Mat _MeanImage = cv::Mat(this->ImageYMax, this->ImageXMax, CV_64F);


    cv::Scalar _ThisNDCMMean, _ThisNDCMStd;
    int SkipperX_Start, SkipperY_Start;

    for (int j = 0; j < this->ImageYMax ; j++) {
        for (int i = 0; i < this->ImageXMax; i++) {
            /*Gather data for a single skip*/
            SkipperX_Start = this->skImgX(StartFrame, i);
            SkipperY_Start = this->skImgY(StartFrame, j);


            cv::Rect _thisSkSeqROI = cv::Rect(SkipperX_Start, SkipperY_Start, EndFrame, 1);
            cv::Mat _thisSkSequence(this->_cvSkImage, _thisSkSeqROI);

            cv::meanStdDev(_thisSkSequence, _ThisNDCMMean, _ThisNDCMStd);

            //if (InvertScale) _MeanImage.at<double>(j, i) = (double) USHRT_MAX - _ThisNDCMMean[0];
            //else _MeanImage.at<double>(j, i) = _ThisNDCMMean[0];

            _MeanImage.at<double>(j, i) = 65535.0 - _ThisNDCMMean[0]; //65535.0 = max value of unsigned short

            ROOTMeanImg.SetBinContent(i, j, _ThisNDCMMean[0]);
            ROOTStdImg.SetBinContent(i, j, _ThisNDCMStd[0]);
        }
    }

    /*Upodate the frame store*/
    //if (FrameStore) this->FrameSequences.push_back(_MeanImage);

    //return _MeanImage;

    /*Update the root container*/
    //int x_range[2] = {3, 54};
    //int y_range[2] = {48, 185};

    //ROOTMeanImg.SetAxisRange(514., 772.,"X");
    //ROOTMeanImg.SetAxisRange(35., 108.,"Y");

    //ROOTMeanImg.SetAxisRange(500., 680.,"X");
    //ROOTMeanImg.SetAxisRange(33., 85.,"Y");

    ROOTMeanImg.GetXaxis()->SetTitle("CCD X (pixels)");
    ROOTMeanImg.GetYaxis()->SetTitle("CCD Y (pixels)");
    //ROOTMeanImg.SetTitle("");

    //ROOTMeanImg.SetContour((sizeof(levels)/sizeof(double)), levels);
    //ROOTMeanImg.SetAxisRange(19300., 19400.,"Z");
    ROOTMeanImg.Write(CombImageName.c_str());
    if (DrawStdev) ROOTStdImg.Write(ShortStdFrameName.c_str());

    return _MeanImage;
}



cv::Mat SKChargeAndNoise::YBaselineCorrect(cv::Mat &inImage, std::string ObjName){

    int XMax = inImage.size().width;
    int YMax = inImage.size().height;

    std::string LongFrameName = "BLFX_"+ObjName;
    std::string ShortFrameName = "BLFX_"+ObjName;
    std::string PlotName = "GrOffsets_"+ObjName;

    /*ROOT Standard images*/
    int rows = this->ImageYMax;
    int cols = this->ImageXMax;
    TH2D ROOTMeanImg(ShortFrameName.c_str(), LongFrameName.c_str(), XMax, 0, XMax, YMax, 0, YMax);

    /*CV Std images*/
    cv::Mat _MeanImageBlFx = cv::Mat(YMax, XMax, CV_64F);


    double _thisRowBaseline;
    double _baselineCorrectedValue;
    cv::Mat _thisRow;

    std::vector<double> colValue;
    std::vector<double> colOffset;

    for (int j = 0; j < YMax ; j++) {

        cv::Rect _thisRowDef = cv::Rect(0,j,XMax,1);
        _thisRow = inImage(_thisRowDef);
        _thisRowBaseline = fMedianPitam(_thisRow);

        colValue.push_back(j);
        colOffset.push_back(_thisRowBaseline);


        for (int i = 0; i < XMax; i++) {
            double _baselineFixed = _thisRow.at<double>(0,i)-_thisRowBaseline;
            ROOTMeanImg.SetBinContent(i, j, _baselineFixed);
            _MeanImageBlFx.at<double>(j,i) = _baselineFixed;
        }
    }

    /*Generate graph of offsets*/
    TGraph grOffsets(colValue.size(),&colValue[0],&colOffset[0]);
    grOffsets.Draw("AP");

    /*Upodate the frame store*/
    //if (frameStore) this->FrameSequences.push_back(_MeanImageBlFx);
    ROOTMeanImg.Write(ShortFrameName.c_str());

    grOffsets.SetName(PlotName.c_str());
    grOffsets.Write();

    return _MeanImageBlFx;
}