#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *ProjectPath = "/Users/lukaszmichalak/Documents/Programowanie/OpenCV Projekt";

void drow(IplImage* in, IplImage* test, char*, int x, int y, int width, int height, int size);

static void drawOptFlowMap(const CvMat* flow, CvMat* cflowmap, int step, double scale, CvScalar color, IplImage* test)
{
    int x, y;
    for( y = 0; y < cflowmap->rows; y += step)
        for( x = 0; x < cflowmap->cols; x += step)
        {
            CvPoint2D32f fxy = CV_MAT_ELEM(*flow, CvPoint2D32f, y, x);
            if(x != cvRound(x+fxy.x) && y != cvRound(y+fxy.y))
                cvLine(test, cvPoint(x,y), cvPoint(cvRound(x+fxy.x), cvRound(y+fxy.y)), color, 10, 8, 0);
            //cvCircle(cflowmap, cvPoint(x,y), 2, color, -1, 8, 0);
        }
}

int main (int argc, char **argv)
{
    int c;
    int i;
    short loop = 1;
    
    IplImage* frame = 0;
    IplImage* gray = 0;
    IplImage* tray = 0;
    
    CvMat* gray2 = 0;
    CvMat* prevgray = 0;
    CvMat* flow = 0;
    CvMat* cflow = 0;
    
    const char* cascade_name = "/Users/lukaszmichalak/Documents/Programowanie/OpenCV Projekt/OpenCV Projekt/haar cascades/haarcascade_mcs_eyepair_big.xml";
    
    CvHaarClassifierCascade* cascade;
    CvMemStorage* storage;
    CvSeq* faces;
  
    cascade = (CvHaarClassifierCascade *) cvLoad(cascade_name, 0, 0, 0);
    cvNamedWindow("Open CV", CV_WINDOW_AUTOSIZE);
    CvCapture* capture = cvCreateCameraCapture(0);
    
    assert(capture != NULL);
  
    while(loop == 1) // główna pętla
    {
        int firstFrame = gray2 == 0;
        
        frame = cvQueryFrame(capture);
        
        // Rozpoznawanie oczu
        gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
        tray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
        storage = cvCreateMemStorage(0);
        cvClearMemStorage(storage);
        cvCvtColor(frame, gray, CV_BGR2GRAY);
        cvSet(tray, cvScalar(0, 0, 0, 0), NULL);
        cvEqualizeHist(gray, gray);

        faces = cvHaarDetectObjects(gray, cascade, storage, 1.11, 4, 0, cvSize (20, 20), cvSize (0, 0));

        for(i = 0; i < (faces ? faces->total : 0); i++)
        {
            CvRect *r = (CvRect *) cvGetSeqElem(faces, i);
            drow(frame, tray, "Eyes", r->x, r->y, r->width, r->height, 20);
        }
        
        cvReleaseImage(&gray);
        
        // Śledzenie
        if(!gray2)
        {
            gray2 = cvCreateMat(tray->height, tray->width, CV_8UC1);
            prevgray = cvCreateMat(gray2->rows, gray2->cols, gray2->type);
            flow = cvCreateMat(gray2->rows, gray2->cols, CV_32FC2);
            cflow = cvCreateMat(gray2->rows, gray2->cols, CV_8UC3);
        }
        
        cvCvtColor(tray, gray2, CV_BGR2GRAY);
        
        if( !firstFrame )
        {
            cvCalcOpticalFlowFarneback(prevgray, gray2, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
            cvCvtColor(prevgray, cflow, CV_GRAY2BGR);
            drawOptFlowMap(flow, cflow, 16, 1.5, CV_RGB(255, 0, 0), frame);
        }
        
        cvShowImage("Open CV", frame);
        
        CvMat* temp;
        CV_SWAP(prevgray, gray2, temp);
    
        // Kontrolowanie
        c = cvWaitKey(2);
        
        switch(c)
        {
            case (int)'Q':
            case (int)'q':
            case 27:
                cvReleaseCapture (&capture);
                cvDestroyWindow ("Open CV");
              
                return 0;
                break;
        }
    }
}

void drow(IplImage* in, IplImage* test, char* text, int x0, int y0, int width, int height, int size)
{
    CvFont font = cvFont(0.8, 2);
    
    char* test2 = "";
    
    char out[100];
    strcat(out, text);
    strcat(out, test2);
    
    cvPutText(in, out, cvPoint(x0, y0-10), &font, cvScalar(0, 255, 0, 1));
    cvRectangle(in, cvPoint(x0, y0), cvPoint(x0+width, y0+height), cvScalar(0, 255, 0, 1), 1, 8, 0);
    cvCircle(test, cvPoint(x0+width/2, y0+height/2), 5, cvScalar(255, 255, 255, 1), 10, 8, 0);
}