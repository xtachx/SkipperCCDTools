//
// Created by Pitam Mitra on 2019-06-09.
//
#include <chrono>
#include "../CoreImageOps.hpp"
#include "fitsio.h"


/*Function needed to convert time points to string*/
static std::string timePointAsString(const std::chrono::system_clock::time_point& tp)
{
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::string ts = std::ctime(&t);
    ts.resize(ts.size()-1);
    return ts;
}



/* Routine to load the FITS image into an OpenCV container */
void SKCCDImage::LoadFits(void ) {

    /*These are the temporary FITS variables we need to open the file
     * fptr -> fits file pointer
     * status, imgtype and nAxis are self explanatory
     * imageSizeXY stores the size of the image
     * fpixelallread is something weird about FITS that I didnt care to read too much except
     *        it needs to be {1,1} to read the entire file
     */
    fitsfile *fptr;
    int status=0, imgtype,naxis;
    long imageSizeXY[2];
    long fpixelallread[2]={1,1};
    int nullValue = 0;;

    /*Open file, get image type, number of dimentions and size*/
    fits_open_file(&fptr,this->inFitsFileName.c_str(),READONLY,&status);
    fits_get_img_type(fptr, &imgtype, &status);
    fits_get_img_dim(fptr,&naxis,&status);
    fits_get_img_size(fptr, 2, imageSizeXY, &status);
    xaxismax = imageSizeXY[0];
    yaxismax = imageSizeXY[1];


    /*Get NDCMs*/
    fits_read_key(fptr, TSHORT, "NDCMS", &this->nChargeMeasurements, NULL, &status);

    /*Set non-skipper image dimensions*/
    this->ImageYMax = yaxismax;
    this->ImageXMax = (xaxismax-2)/this->nChargeMeasurements;


    /*Get the Amplifier direction*/
    char Ampl[15];
    fits_read_key(fptr, TSTRING, "AMPL", Ampl, NULL, &status);
    this->Amplifier = std::string(Ampl);

    /*assign the cv::Mat image, and fill its buffer with the FITS image*/
    this->_cvSkImage = cv::Mat(yaxismax, xaxismax, CV_32S);
    this->_cvSkImageData = (double*) _cvSkImage.data; //access to cv::Mat's buffer
    fits_read_pix( fptr, TINT, fpixelallread, xaxismax*yaxismax, 0, _cvSkImageData,&nullValue,&status);


    /*Errors - if any*/
    if(status)
        fits_report_error(stderr, status);

    /*Close fits file*/
    fits_close_file(fptr, &status);

}


/*Function to write an SK Merged image as a FITS file.*/
void SKCCDImage::SaveFitsSK(cv::Mat ImageToSave, std::string outFileName){

    /*These are the temporary FITS variables we need to open the file
     * fptr -> fits file pointer
     * status, imgtype and nAxis are self explanatory
     * imageSizeXY stores the size of the image
     * fpixelallread is something weird about FITS that I didnt care to read too much except
     *        it needs to be {1,1} to read the entire file
     */
    fitsfile *fptr;       /* pointer to the FITS file; defined in fitsio.h */
    int status, nAxis=2;
    long fpixel[2] = {1,1};
    long nPixelsToWrite;
    long imageSizeXY_Out[2] = { this->ImageXMax, this->ImageYMax };
    nPixelsToWrite = imageSizeXY_Out[0] * imageSizeXY_Out[1];

    status = 0;         /* initialize status before calling fitsio routines */
    fits_create_file(&fptr, outFileName.c_str(), &status);

    /*This section copies the header from the original file to the new one*/
    //fitsfile *fptr_infile;
    //fits_open_file(&fptr_infile,this->inFitsFileName.c_str(),READONLY,&status);
    //fits_copy_header(fptr_infile, fptr,  &status);
    //fits_close_file(fptr_infile, &status);
    /* ------------- */

    fits_create_img(fptr, LONG_IMG, nAxis, imageSizeXY_Out, &status);
    int nullValue=0;

    /*Write processed comment*/
    std::string sKFixedCmt = "This image was taken by a DAMIC Skipper CCD. "
                             "The image was reconstructed by taking the median of the charge readouts. "
                             "The various settings used are stored as keys in the FITS file."
                             "Processed by DAMICMRecon - Pitam Mitra @ UW. If you have questions, please send them to pitamm@uw.edu";
    fits_write_comment(fptr, sKFixedCmt.c_str(), &status);


    /* Write the recon related keywords*/
    std::chrono::system_clock::time_point _processTime = std::chrono::system_clock::now();
    //fits_write_key(fptr, TSTRING, "RTime", (char*) _processTime.c_str(), "Time of recon processing", &status);
    fits_write_key(fptr, TINT, "RIMGOFST", &this->CCDImageOffset, "Image offset (usually 2 cols at the beginning of the file).", &status);
    fits_write_key(fptr, TINT, "RIGNDCMS", &this->nIgnoreFirstNDCMs, "How many NDCMs to ignore in each pixel for recon.", &status);


    /*Write image*/
    double *_cvMergedImageData = (double*) ImageToSave.data;
    fits_write_pix(fptr, TDOUBLE, fpixel, nPixelsToWrite, _cvMergedImageData, &status);

    /*Done*/
    fits_close_file(fptr, &status);
    fits_report_error(stderr, status);
}
