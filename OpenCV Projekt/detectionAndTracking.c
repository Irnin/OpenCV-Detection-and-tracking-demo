#include "detectionAndTracking.h"

void drow(IplImage* in, IplImage* tray, char* text, int x0, int y0, int width, int height, int size)
{
    CvFont font = cvFont(0.8, 2);
    
    long position;
    char *out = malloc(60 * sizeof(char));
    char *positionX = malloc(60 * sizeof(char));
    char *positionY = malloc(60 * sizeof(char));
    
    strcat(out, text);
    strcat(out, " x: ");
    
    position = x0+width/2;
    sprintf(positionX, "%ld", position);
    strcat(out, positionX);
    
    strcat(out, " y: ");
    
    position = y0+height/2;
    sprintf(positionY, "%ld", position);
    strcat(out, positionY);
    
    cvPutText(in, out, cvPoint(x0, y0-10), &font, cvScalar(0, 255, 0, 1));
    cvRectangle(in, cvPoint(x0, y0), cvPoint(x0+width, y0+height), cvScalar(0, 255, 0, 1), 1, 8, 0);
    cvCircle(tray, cvPoint(x0+width/2, y0+height/2), 5, cvScalar(255, 255, 255, 1), 10, 8, 0);
    
    free(out);
    free(positionX);
    free(positionY);
}

void drawOptFlowMap(CvMat* flow, CvMat* cflowmap, int step, double scale, CvScalar color, IplImage* test)
{
    int x, y;
    for( y = 0; y < cflowmap->rows; y += step)
    {
        for( x = 0; x < cflowmap->cols; x += step)
        {
            CvPoint2D32f fxy = CV_MAT_ELEM(*flow, CvPoint2D32f, y, x);
            if(x != cvRound(x+fxy.x) && y != cvRound(y+fxy.y))
                cvLine(test, cvPoint(x,y), cvPoint(cvRound(x+fxy.x), cvRound(y+fxy.y)), color, 10, 8, 0);
        }
    }
}
