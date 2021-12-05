#include <highgui.h>
#include <cv.h>
#include <stdio.h>

struct thingToTrack
{
    char* filePath;
    char* name;
};

void drawDetection(IplImage*, IplImage*, char*, int, int, int, int, int);
void drawTracking(CvMat*, CvMat*, int, double, CvScalar, IplImage*);
