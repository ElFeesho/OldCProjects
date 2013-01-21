#ifndef __CAMERA_H__
#define __CAMERA_H__

void camera_init();
void camera_open(const char *dev_name, int width, int height);
void camera_shutdown();
gboolean camera_start_capture();
gboolean camera_stop_capture();
char *camera_read_image();
gboolean camera_process_image(unsigned char *p);
gboolean camera_set_gain(float v);

#endif
