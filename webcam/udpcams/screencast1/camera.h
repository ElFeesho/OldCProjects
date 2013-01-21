#ifndef __CAMERA_H__
#define __CAMERA_H__

void camera_init();
void camera_open(const char *dev_name, int width, int height);
void camera_shutdown();
char *camera_read_image();

#endif
