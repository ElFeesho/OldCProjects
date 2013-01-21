#ifndef __CAMERA_H__
#define __CAMERA_H__

int camera_init();
int camera_open(const char *dev_name, int width, int height);
void camera_shutdown();
unsigned char *camera_read_image();
int camera_set_contrast(int value);
int camera_set_brightness(int value);

#endif
