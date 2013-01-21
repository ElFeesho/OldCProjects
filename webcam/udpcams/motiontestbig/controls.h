#ifndef __CONTROLS_H__
#define __CONTROLS_H__

int controls_get_max_gain(int fd);
int controls_get_gain(int fd);
void controls_set_gain(int fd, int v);
void controls_set_blue_balance(int fd, int v);
int controls_get_blue_balance(int fd);
void controls_set_white_balance_blue(int fd, int v);
int controls_get_white_balance_blue(int fd);
void controls_set_white_balance_red(int fd, int v);
int controls_get_white_balance_red(int fd);
void controls_set_auto_adjust(int fd, int v);
int controls_get_auto_adjust(int fd);
void controls_set_red_balance(int fd, int v);
int controls_get_red_balance(int fd);
void controls_set_vflip(int fd, char on);
int controls_get_vflip(int fd);
void controls_set_exposure(int fd, int v);
int controls_get_exposure(int fd);
int controls_get_jpegcompr(int fd);
void controls_set_jpegcompr(int fd, int v);

#endif
