#include <sys/ioctl.h>		// for ioctl
#include <sys/types.h>		// req'd for videodev2.h
#include <asm/types.h>		// req'd for videodev2.h
#include <errno.h>		// for errno
#include <stdio.h>		// for fprintf
#include <string.h>		// for memset
#include <assert.h>

#include "controls.h"

int controls_get_max_gain(int fd)
{
	printf("Stub get max gain\n");
	return 0;
}


int controls_get_gain(int fd)
{
	printf("Stub get gain\n");
	return 0;
}


void controls_set_gain(int fd, int v)
{
	printf("Stub set gain\n");
}
