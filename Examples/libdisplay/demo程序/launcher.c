#include <stdio.h>
#include "libarkcmn/ark_api.h"

int dvr_fd = -1;
int fb_dev_fd = -1;

int main(void)
{
	int demo_flag = 1;
	int width, height;
	unsigned char mpcmd[128];

	//switch display to app framebuffer
	system("echo 23 > /proc/display");

	if (1/*MCU detect exit car back*/) {
		//close fast cark back display
		system("echo \"21 0\" > /proc/display");

		//display framebuffer(when fast car back, UI(fb0) is hide)
		system("echo \"24 1\" > /proc/display");
	}

	arkapi_get_screen_size(&width, &height);

	printf("screen %dx%d.\n", width, height);
	sprintf(mpcmd, "./mplayer -geometry %dx%d+0+0 -vo customfb -ao alsa -cache 1024 -quiet -slave -input file=/tmp/mpfifo -tvout 1.mkv -loop 0 &", width, height);

	//simulate start mplayer in the background
	system("mkfifo /tmp/mpfifo");
	system(mpcmd);
	sleep(5);
	

	dvr_fd = arkapi_open_dvr();
	if (dvr_fd < 0) {
		printf("open dvr device failure.\n");
		return -1;
	}	

	if (arkapi_dvr_config(dvr_fd, DVR_SOURCE_CAMERA, width, height, 0) < 0) {
		printf("config dvr failure.\n");
		return -1;	
	}

	srand((unsigned int)time(NULL));

	while(1) {
		//get carback in msg from mcu
		if (demo_flag) {
			//notify mplayer to giveup video display resource
			system("echo giveupvideo > /tmp/mpfifo");

			//only for avoid video display resource confilict
			fb_dev_fd = arkapi_open_video_fb();
			if (fb_dev_fd < 0) {
				printf("open video fb device failure.\n");
				return -1;
			}

			arkapi_dvr_start(dvr_fd);
			demo_flag = 0;
		} 
		//get carback quit msg from mcu
		else if (!demo_flag) {
			arkapi_dvr_stop(dvr_fd);
			arkapi_close_video_fb(fb_dev_fd);
			//notify mplayer to retake video display resource
			system("echo takevideo > /tmp/mpfifo");			
			demo_flag = 1;		
		}

		sleep(rand() % 10 + 3);
	}

	arkapi_close_dvr(dvr_fd);

	return 0;
}
