#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <gtk/gtk.h>
#include <stdint.h>

#define USER_INK 0x01
#define USER_VOICE 0x02

extern GtkWidget *client_get_widget();
extern void client_add(char *nick, int32_t id);
extern void client_change(char *nname, int32_t id);
extern void client_set_name(char *nname, int32_t id);
extern void client_set_website(char *website, int32_t id);
extern void client_set_email(char *email, int32_t id);
extern char *client_get_name(int32_t id);
extern void client_remove(int32_t id);
extern void client_clear_list();
extern int client_get_status(int32_t id);
extern int client_set_status(int32_t id, int status);
#ifdef LOG
extern char client_get_logged(int32_t id);
extern void client_set_logged(int32_t id);
#endif

#endif



