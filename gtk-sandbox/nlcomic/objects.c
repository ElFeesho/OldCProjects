#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

typedef struct _object_node
{
	int x;
	int y;
	int w;
	int h;
	double rot;
	double scale;
	char *emotion;
	int action;
	int font_size;
	char dir;
	char *text; // If this isn't NULL the object is text!
	char *xml_file; // If this isn't NULL the object is an.. object
	void *next;
} object_node;

object_node *object_list[16];

void object_add(int panel, int x, int y, char *xml_file, char *text)
{
	if(object_list[panel]==NULL)
	{
		object_list[panel] = malloc(sizeof(object_node));
		object_list[panel]->x = x;
		object_list[panel]->y = y;
		object_list[panel]->w = 96; //NOT PERMA
		object_list[panel]->h = 96; //NOT PERMA
		object_list[panel]->xml_file = xml_file;
		object_list[panel]->text = text;
		object_list[panel]->font_size = 10;
		object_list[panel]->dir = 0;
		object_list[panel]->rot = 0.0;
		object_list[panel]->scale = 1.0;
		object_list[panel]->emotion = NULL;
		object_list[panel]->action = 0;
		object_list[panel]->next = NULL;
	}
	else
	{
		object_node *temp = malloc(sizeof(object_node));
		temp->x = x;
		temp->y = y;
		temp->w = 96;
		temp->h = 96;
		temp->xml_file = xml_file;
		temp->text = text;
		temp->font_size = 10;
		temp->dir = 0;
		temp->rot = 0.0;
		temp->scale = 1.0;
		temp->emotion = NULL;
		temp->action = 0;
		temp->next = object_list[panel];
		object_list[panel] = temp;
	}
}
