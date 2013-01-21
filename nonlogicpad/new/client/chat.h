#ifndef __CHAT_H__
#define __CHAT_H__

#include <gtk/gtk.h>
#include <stdint.h>

extern void chat_append(char *message);
extern GtkWidget *chat_get_history_widget();
extern void chat_add_msg(int32_t id, char *msg, int32_t len);
extern void chat_add_action(int32_t id, char *action, int32_t len);

#endif

