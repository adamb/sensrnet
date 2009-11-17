// gab an image and ftp it to http://sensr.net
//
// author: adam beguelin
// created: 2009-11-17

#include <stdio.h>
#include <string.h>

#include <cv.h>
#include <highgui.h>

#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif


IplImage* get_image(CvCapture* cap)
{
	IplImage* image;
	IplImage* out = 0;
	
	image = cvQueryFrame( cap );	
	// resize the image
	out = cvCreateImage(cvSize( 640,480 ), image->depth, image->nChannels);
	cvResize(image,out);
	
	return(out);
	
}

int main(int argc, char *argv[]) {
    CvCapture* capture;
	IplImage* img;

	if (argc != 4) {
		fprintf(stderr, "usage: %s <ftpserver> <username> <password>\n", argv[0]);
		return -1;
	}

	char *serv = argv[1];
	char *user = argv[2];
	char *pass = argv[2];
	
	capture = cvCaptureFromCAM( 0 );
	img = get_image(capture);
	cvSaveImage("test.jpg",img);
//	ftp_image(img,serv,user,pass);

}