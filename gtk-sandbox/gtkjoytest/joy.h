#ifndef __JOY_H__
#define __JOY_H__

#include <gtk/gtk.h>

extern void joy_open(char *device);
extern gpointer joy_poll(void);
extern int *joy_get_axis(void);
extern int *joy_get_button(void);
#endif

