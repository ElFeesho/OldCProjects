#ifndef __WHITEB_H__
#define __WHITEB_H__

#include "question.h"

extern void whiteb_show(struct question *data);
extern void whiteb_change_tool(GtkWidget *widget, gpointer user_data);
extern void whiteb_clear(GtkWidget *canvas, gpointer user_data);

#endif
