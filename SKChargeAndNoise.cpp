#include <opencv2/opencv.hpp>
#include "CoreImageOps.hpp"


void main () {


    //double _ThisNDCMMean, _ThisNDCMStd;

    /* CV Standard images */
    this->MeanImage = cv::Mat(this->ImageYMax, this->ImageXMax, CV_64F);
    this->StdevImage = cv::Mat(this->ImageYMax, this->ImageXMax, CV_64F);

    /*First 2 frames*/
    cv::Mat Frame1 = this->MakeMeanImgOfFrames(skImage, 1,2, "Frame1", false, true);
    cv::Mat Frame2 = this->MakeMeanImgOfFrames(skImage, 2,3, "Frame2", false, true);

    /*Last 2 frames*/
    cv::Mat LFrame1 = this->MakeMeanImgOfFrames(skImage, this->nChargeMeasurements-2,this->nChargeMeasurements-1, "LFrame1", false, true);
    cv::Mat LFrame2 = this->MakeMeanImgOfFrames(skImage, this->nChargeMeasurements-3,this->nChargeMeasurements-2, "LFrame2", false, true);

    /*Difference of the frames*/
    this->DiffLastFirst = LFrame1 - Frame1;
    this->DiffSecondLast = LFrame1 - Frame2;
    this->DiffSecondFirst = Frame2 - Frame1;
    this->OpenCVToTH2(this->DiffLastFirst,"DeltaFirstLast");
    this->OpenCVToTH2(this->DiffSecondLast,"DeltaSecondLast");
    this->OpenCVToTH2(this->DiffSecondFirst,"DeltaFirstSecond");

    /*10, 50, 100, 300, 500, 800 and 900 images*/
    std::vector<int> _MakeMeanImages {10,50,100,300,500,800,900,1500,1900,3000,3900};
    std::vector<double> SpectrumWidth, SpectrumWidthErr;

    for (int i: _MakeMeanImages){

        int _startFrame = 5;
        int _endFrame = _startFrame+i;

        /*If NDCMs < the number of frames being averaged, then break */
        if (this->nChargeMeasurements < _endFrame) break;

        /*Decide on a name for the frame*/
        std::string _thisAvgIterName = "SequenceAvg_"+std::to_string(i);

        cv::Mat _thisAvgIter = this->MakeMeanImgOfFrames(skImage, _startFrame,_endFrame, _thisAvgIterName);
        this->OpenCVToTH2(_thisAvgIter,_thisAvgIterName);

        /*Make the image histogram*/
        std::string _thisAvgSpectrumName = "SKSpectrumAvg_"+std::to_string(i);
        int SpecYLength = this->ImageYMax-2-1;
        int SpecXLength = this->ImageXMax-80-5;
        TH1D ThisSpectrum = this->MKSpectrum(_thisAvgIter,5,1,SpecXLength,SpecYLength,_thisAvgSpectrumName);
        /*Analysis of the spectra*/
        std::vector<double> SpecPeaks = this->FindPeaksSpectra(ThisSpectrum, "ThisSpecPeaks");
        PeakFitParameters SpecFitPars = this->FitSpectrumPeak(ThisSpectrum, SpecPeaks[0], i, "FullSpecFit");

        SpectrumWidth.push_back(SpecFitPars.sigma);
        SpectrumWidthErr.push_back(SpecFitPars.delta_sigma);
    }


//    cv::Mat _10Frames = this->MakeMeanImgOfFrames(skImage, 5,15, "SequenceAvg_10");

    /*Discretize!*/
    //this->DiscretizeElectrons(skImage, 5,15, "ElectronFSAvg",19389.7, 11.6);



    /*Full spectrum*/
    int FullSpectrumNDCMStart = 5;
    int FullSpectrumNDCMEnd= this->nChargeMeasurements>100 ? this->nChargeMeasurements-50 : this->nChargeMeasurements-5;
    std::string FSpectrumImgName = this->nChargeMeasurements>100 ? "FSpectrumAvg" : "FSpectrumAvg_Warn";
    cv::Mat MeanImageFS = this->MakeMeanImgOfFrames(skImage, FullSpectrumNDCMStart,FullSpectrumNDCMEnd, FSpectrumImgName, true);


    /*Baseline corrected image*/
    cv::Mat MeanImageBlFx = this->YBaselineCorrect(MeanImageFS,"AllFramesMean");

    /*Additional Spectra*/
    int SpecYLength = this->ImageYMax-2-1;
    int SpecXLength = this->ImageXMax-80-5;

    this->MKSpectrum(MeanImageFS,5,1,SpecXLength,SpecYLength,"FullSpectrum");
    this->MKSpectrum(MeanImageFS,5,this->ImageYMax-7,SpecXLength,5,"FullSpectrumStrip");

    this->MKSpectrum(MeanImageBlFx,5,1,SpecXLength,SpecYLength,"BLSpectrum");
    this->MKSpectrum(MeanImageBlFx,5,this->ImageYMax-7,SpecXLength,5,"BLSpectrumStrip");

    if (MeanImageFS.size().height>60)
        this->MKSpectrum(MeanImageFS,50,50,this->ImageXMax-100,this->ImageYMax-60,"FullSpectrum_after50");



    /*Save the full spec image as FITS file for faster viewing with DS9*/
    std::string MeanOutFitsName = "FS_Avg_"+this->ImageFileStem+".fits";
    this->SaveFitsSK(MeanImageFS,MeanOutFitsName);


    /*Graph of spectrum width*/
    std::vector<double> ImageNDCMS(_MakeMeanImages.begin(), _MakeMeanImages.end());
    TGraphErrors SpectrumWidthPlot(SpectrumWidth.size(), &ImageNDCMS[0], &SpectrumWidth[0],0,&SpectrumWidthErr[0]);
    SpectrumWidthPlot.SetTitle("Sigma size vs NDCMs;NDCMs;Sigma");

    TF1 OneOverRootN("OneRN","[0]/pow(x,0.5)",5,this->nChargeMeasurements);
    SpectrumWidthPlot.Fit("OneRN");
    SpectrumWidthPlot.Write("PeakSigma");


}
