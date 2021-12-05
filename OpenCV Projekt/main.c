#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "detectionAndTracking.h"

//Path to main project
#define projectPath "/Users/lukaszmichalak/Documents/Programowanie/C/OpenCV Projekt/OpenCV Projekt/"

//1: English 2: Polish
#define languageCode 1

int main (int argc, char **argv)
{
    int i;
    int keyboardCharInput;
    CvHaarClassifierCascade* cascade;
    
    IplImage* frame = 0;
    IplImage* gray = 0;
    IplImage* tray = 0;
    
    CvMat* gray2 = 0;
    CvMat* prevgray = 0;
    CvMat* flow = 0;
    CvMat* cflow = 0;
    
    CvMemStorage* storage;
    CvSeq* faces;
    
    struct thingToTrack* haarCascade = malloc(sizeof(struct thingToTrack));
    
    haarCascade -> filePath = projectPath "haarcascade_mcs_eyepair_big.xml";
    
    #if languageCode == 1
        haarCascade -> name = "Eyes";
    #elif languageCode == 2
        haarCascade -> name = "Oczy";
    #endif
  
    cascade = (CvHaarClassifierCascade *) cvLoad(haarCascade -> filePath, 0, 0, 0);
    cvNamedWindow("Open CV", CV_WINDOW_AUTOSIZE);
    CvCapture* capture = cvCreateCameraCapture(0);
    
    assert(capture != NULL);
    storage = cvCreateMemStorage(0);
  
    // Main loop of the program
    for(;;)
    {
        frame = cvQueryFrame(capture);
        gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
        tray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
        
        if(!gray2)
        {
            gray2 = cvCreateMat(tray->height, tray->width, CV_8UC1);
            prevgray = cvCreateMat(gray2->rows, gray2->cols, gray2->type);
            flow = cvCreateMat(gray2->rows, gray2->cols, CV_32FC2);
            cflow = cvCreateMat(gray2->rows, gray2->cols, CV_8UC3);
        }

        cvClearMemStorage(storage);
        cvCvtColor(frame, gray, CV_BGR2GRAY);
        cvSet(tray, cvScalar(0, 0, 0, 0), NULL);
        cvEqualizeHist(gray, gray);

        faces = cvHaarDetectObjects(gray, cascade, storage, 1.11, 4, 0, cvSize (20, 20), cvSize (0, 0));

        for(i = 0; i < (faces ? faces->total : 0); i++)
        {
            CvRect *r = (CvRect *) cvGetSeqElem(faces, i);
            drawDetection(frame, tray, haarCascade -> name, r->x, r->y, r->width, r->height, 20);
        }
        
        cvReleaseImage(&gray);
        
        int firstFrame = gray2 == 0;
        cvCvtColor(tray, gray2, CV_BGR2GRAY);
        
        if( !firstFrame )
        {
            cvCalcOpticalFlowFarneback(prevgray, gray2, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
            cvCvtColor(prevgray, cflow, CV_GRAY2BGR);
            drawTracking(flow, cflow, 16, 1.5, CV_RGB(255, 0, 0), frame);
        }

        CvMat* temp;
        CV_SWAP(prevgray, gray2, temp);
        
        // Control
        keyboardCharInput = cvWaitKey(2);
        switch(keyboardCharInput)
        {
            case (int)'Q':
            case (int)'q':
            case 27:
                free(haarCascade);
                cvReleaseCapture (&capture);
                cvDestroyWindow ("Open CV");
              
                return 0;
                break;
        }
        
        cvShowImage("Open CV", frame);
    }
}
