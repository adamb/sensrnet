// gab an image and ftp it to http://sensr.net
//
// author: adam beguelin
// created: 2009-11-17
//
// ftp upload code based on http://curl.haxx.se/libcurl/c/ftpupload.html
//

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


// upload the given file to the server
int ftp_image(char* file, char* serv, char* user, char* pass)
{
	
  CURL *curl;
  CURLcode res;
  FILE *hd_src;
  struct stat file_info;
  curl_off_t fsize;

  char remote_url[80];
  snprintf(remote_url, 80, "ftp://%s/%s", serv,file);

  char creds[80];
  snprintf(creds, 80, "%s:%s", user, pass);

  struct curl_slist *headerlist=NULL;
  char buf[80];
  snprintf(buf, 80, "RNFR %s", file);

  /* get the file size of the local file */
  if(stat(file, &file_info)) {
    printf("Couldnt open '%s': %s\n", file, strerror(errno));
    return 1;
  }
  fsize = (curl_off_t)file_info.st_size;

  printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fsize);

  /* get a FILE * of the same file */
  hd_src = fopen(file, "rb");

  /* In windows, this will init the winsock stuff */
  curl_global_init(CURL_GLOBAL_ALL);

  /* get a curl handle */
  curl = curl_easy_init();
  if(curl) {
    /* build a list of commands to pass to libcurl */
    headerlist = curl_slist_append(headerlist, buf);

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	
    /* enable uploading */
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    /* specify target */
    curl_easy_setopt(curl,CURLOPT_URL, remote_url);

	/* speicfy the username and password */
	curl_easy_setopt(curl, CURLOPT_USERPWD, creds);

    /* pass in that last of FTP commands to run after the transfer */
    curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

    /* now specify which file to upload */
    curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

    /* Set the size of the file to upload (optional).  If you give a *_LARGE
       option you MUST make sure that the type of the passed-in argument is a
       curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
       make sure that to pass in a type 'long' argument. */
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)fsize);

    /* Now run off and do what you've been told! */
    res = curl_easy_perform(curl);

    /* clean up the FTP commands list */
    curl_slist_free_all (headerlist);

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  fclose(hd_src); /* close the local file */

  curl_global_cleanup();

  return 0;
}

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
	char *pass = argv[3];
	char fn[80];
	
	capture = cvCaptureFromCAM( 0 );
	
	int cnt = 0;
	while(true){
		img = get_image(capture);
		snprintf(fn, 80, "test%d.jpg",cnt++);
		cvSaveImage(fn,img);
		cvFree(&img);
		ftp_image(fn,serv,user,pass);
		unlink(fn);
		sleep(1);
    }

}