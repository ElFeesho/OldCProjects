#ifndef __CAMERA_H__
#define __CAMERA_H__

void camera_init();
void camera_open(const char *dev_name, int width, int height);
void camera_shutdown();
gboolean camera_start_capture();
gboolean camera_stop_capture();
unsigned char *camera_read_image();
gboolean camera_process_image(unsigned char *p);
void camera_set_gain(int v);
void camera_set_red_balance(int v);
void camera_set_blue_balance(int v);
void camera_set_white_balance_blue(int v);
void camera_set_white_balance_red(int v);
void camera_set_exposure(int v);
void camera_set_auto_adjust(int v);
void camera_set_vflip(int v);

#endif
