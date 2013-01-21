#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <gtk/gtk.h>
#include <stdint.h>

extern GtkWidget *client_get_widget();
extern void client_add(char *nick, int32_t id);
extern void client_change(char *nname, int32_t id);
extern char *client_get_name(int32_t id);
extern void client_remove(int32_t id);
extern void client_clear_list();

#endif



