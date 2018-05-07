#include <stdio.h>
#include <stdlib.h>
#include "libarkcmn/ark_api.h"

int main(int argc, char *argv[])
{
	int gui_tvout;

	if (argc != 2) {
		printf("Usage: %s on(1)/off(0).\n", argv[0]);
		return -1;
	}	

	char *endptr;
	gui_tvout = strtoul(argv[1], &endptr, 10);

	arkapi_gui_tvout(gui_tvout);

	return 0;
}
