#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "libarkcmn/ark_api.h"

int dvr_fd = -1;
int fb_dev_fd = -1;
int g_exit = 0;

void exit_sighandler(int signo)
{
	printf("exit_handler\n");
	g_exit = 1;
}

int main(int argc, char *argv[])
{
	if (argc != 5) {
		printf("Usage: %s channel(0:camera,1:auxin,2:dvd) out_width out_height tvout) .\n", argv[0]);
		return -1;
	}	

	dvr_fd = arkapi_open_dvr();
	if (dvr_fd < 0) {
		printf("open dvr device failure.\n");
		return -1;
	}

	char *endptr;
	int channel = strtoul(argv[1], &endptr, 10);
	int width = strtoul(argv[2], &endptr, 10);
	int height = strtoul(argv[3], &endptr, 10);
	int tvout = strtoul(argv[4], &endptr, 10);

	if (arkapi_dvr_switch_channel(dvr_fd, channel) < 0) {
            printf("switch dvr failure.\n");
			return -1;
    }
	if (arkapi_dvr_config(dvr_fd, channel, width, height, tvout) < 0) {
		printf("config dvr failure.\n");
		return -1;	
	}	

	//only for avoid video display resource confilict
	fb_dev_fd = arkapi_open_video_fb();
	if (fb_dev_fd < 0) {
		printf("open video fb device failure.\n");
		return -1;
	}

	arkapi_dvr_start(dvr_fd);

	signal(SIGTERM,exit_sighandler);

	char buf[32];
	while (!g_exit) {
//respond kill command need comment code below 
#if 1
		printf("signal=%d\n", arkapi_dvr_detect_signal(dvr_fd));
		scanf("%s", buf);
		if (!strcmp(buf, "quit") || !(strcmp(buf, "exit"))) {
			printf("exit!\n");
			break;
		} else if (!strcmp(buf, "stop")) {
			printf("stop dvr.\n");
			arkapi_dvr_stop(dvr_fd);
		} else if (!strcmp(buf, "start")) {
			printf("start dvr.\n");
			arkapi_dvr_start(dvr_fd);
		} else if (!strcmp(buf, "opentvout")) {
			printf("enable tvout.\n");
			arkapi_dvr_tvout(dvr_fd, 1);
		} else if (!strcmp(buf, "closetvout")) {
			printf("disable tvout.\n");
			arkapi_dvr_tvout(dvr_fd, 0);
		}
#endif
		//usleep(200000);
	}

	arkapi_dvr_stop(dvr_fd);
	arkapi_close_dvr(dvr_fd);
	arkapi_close_video_fb(fb_dev_fd);

	return 0;
}
