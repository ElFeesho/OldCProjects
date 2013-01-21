#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "asm.h"

static GtkWidget *notebook, *regs_table, *regs[16], *stackp, *code_view, *code_list, *var_list, *stdout_tv;
static GtkListStore *code_list_store;
static GtkListStore *var_list_store;

/*
	Some widgets that'll be changed by various effects, so they are unit-global.
	
	code_view is a text view which will be where the user places assembler code
	
	code_list is the treeview containing each line of code (no comments) and will highlight
	to show the flow of the program.
	
	stdout_tv is going to be the 'terminal'. it will hold any textual output and read
	input.
	
	code_view, code_list and stdout will be in the "notebook" notebook.
	
	
*/



/* Program Counter */
static int pc;
static int timeout_id = 0;

/* Helper functions */

static gboolean code_run(gpointer user_data)
{
	pc = asm_step();
	if(pc == -1)
	{
		timeout_id = 0;
		return FALSE;
	}
	return TRUE;
}

static char **gui_get_asm_lines(int *lc)
{
	GtkTextBuffer *code_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code_view));
	
	GtkTextIter start, end;
	
	gtk_text_buffer_get_bounds(code_buf, &start, &end);
	
	char *text = gtk_text_buffer_get_text(code_buf, &start, &end, FALSE);
	
	int line_count = 0;
	int i = 0;
	
	/* Find out how many lines we need to store in an array */
	for(i=0;i<strlen(text);i++)
	{
		if(text[i] == ';')
		{
			if(i>0)
			{
				if(text[i-1]!='\n')
				{
					continue;
				}
			}
			char *nlp = strchr(text+i,'\n');
			if(nlp!=NULL)
				i = nlp-text;
		}
		else if(text[i]=='\n')
			line_count++;
	}
	/* Last line */
	line_count++;
	char **return_lines = g_malloc0(sizeof(char*)*line_count);
	
	int line = 0;
	for(i=0;i<strlen(text);i++)
	{
		if(text[i] == '\n')
			continue;
		if(text[i] == ';')
		{
			if(i>0)
			{
				if(text[i-1]!='\n')
				{
					continue;
				}
			}
			char *nlp = strchr(text+i,'\n');
			if(nlp!=NULL)
				i = nlp-text;
		}
		else
		{
			/* Remove any whitespace before the line */
			while(isspace(text[i])) i++;
			return_lines[line] = g_strdup(text+i);
			char *nlp = strchr(return_lines[line],'\n');
			if(nlp!=NULL)
				nlp[0] = '\0';
			i+=strlen(return_lines[line]);
			line++;
			
		}
	}
	
	/* TODO: sanitise strings to remove comments */
	
	*lc = line;
		
	return return_lines;
}

static void populate_code_list(void)
{
	gtk_list_store_clear(code_list_store);
	
	int line_count = 0;
	char **lines = gui_get_asm_lines(&line_count);

	GtkTreeIter next_line;
	int i = 0;
	for(i = 0;i<line_count;i++)
	{
		gtk_list_store_append(code_list_store, &next_line);
		gtk_list_store_set(code_list_store, &next_line, 0, lines[i], -1);
	}
	
	asm_reset_vm();
	asm_set_prog(lines, line_count);
}

static void populate_var_list(void)
{
	asm_var *var_list = asm_get_var_list();
	gtk_list_store_clear(var_list_store);

	GtkTreeIter next_line;
	int i = 0;
	for(;var_list!=NULL;var_list=var_list->next)
	{
		gtk_list_store_append(var_list_store, &next_line);
		if(var_list->type == 0 && var_list->size>1)
			gtk_list_store_set(var_list_store, &next_line, 0, var_list->name, 2, var_list->ptr, 1, g_strdup_printf("%X",var_list->sptr), -1);
		else if(var_list->type == 0)
			gtk_list_store_set(var_list_store, &next_line, 0, var_list->name, 2, g_strdup_printf("%d",(unsigned char*)var_list->ptr), 1, g_strdup_printf("%X",var_list->sptr), -1);
		else if(var_list->type == 1)
			gtk_list_store_set(var_list_store, &next_line, 0, var_list->name, 2, g_strdup_printf("%d",(unsigned short*)var_list->ptr), 1, g_strdup_printf("%X",var_list->sptr), -1);
	}
}

static void gui_code_list_create(void)
{
	code_list = gtk_tree_view_new();
	
	/* Create the code list store- it'll store just the line */
	code_list_store = gtk_list_store_new(1,G_TYPE_STRING);
	GtkCellRenderer *code_list_rend = gtk_cell_renderer_text_new();
	
	/* Set the 1st column to be viewable in the treeview */
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(code_list),-1,"Code",code_list_rend,"text",0,NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(code_list),GTK_TREE_MODEL(code_list_store));
}

static void gui_var_list_create(void)
{
	var_list = gtk_tree_view_new();
	
	/* Create the code list store- it'll store just the line */
	var_list_store = gtk_list_store_new(3,G_TYPE_STRING,G_TYPE_STRING, G_TYPE_STRING);
	GtkCellRenderer *var_list_rend = gtk_cell_renderer_text_new();
	
	/* Set the 1st column to be viewable in the treeview */
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(var_list),-1,"Variable",var_list_rend,"text",0,NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(var_list),-1,"Address",var_list_rend,"text",1,NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(var_list),-1,"Value",var_list_rend,"text",2,NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(var_list),GTK_TREE_MODEL(var_list_store));
}

/* Callback Definitions */

static gboolean window_delete_event(GtkWidget *window, gpointer user_data)
{
	gtk_main_quit();
	return FALSE;
}

static void run_clicked(GtkWidget *run, gpointer user_data)
{
	if(timeout_id != 0)
		return;
	pc = 0;
	
	populate_code_list();
	populate_var_list();
	timeout_id = g_timeout_add(125, (GSourceFunc)code_run, NULL);
}

static void stop_clicked(GtkWidget *stop, gpointer user_data)
{
	if(timeout_id!=0)
	{
		g_source_remove(timeout_id);
		timeout_id = 0;
	}
}

void gui_update()
{
	unsigned short *reg_vals = g_malloc0(sizeof(short)*8);
	asm_get_regs(reg_vals);
	
	int i = 0;
	for(i = 0;i<7;i++)
		gtk_entry_set_text(GTK_ENTRY(regs[i]), g_strdup_printf("%u", reg_vals[i]));
	
	/* Put that stack in Hex format */
	gtk_entry_set_text(GTK_ENTRY(regs[7]), g_strdup_printf("%X", reg_vals[7]));
	/* Special Cases */
	gtk_entry_set_text(GTK_ENTRY(regs[8]), g_strdup_printf("%u", (reg_vals[0]&0xff00)>>8));
	gtk_entry_set_text(GTK_ENTRY(regs[9]), g_strdup_printf("%u", (reg_vals[1]&0xff00)>>8));
	gtk_entry_set_text(GTK_ENTRY(regs[10]), g_strdup_printf("%u", (reg_vals[2]&0xff00)>>8));
	gtk_entry_set_text(GTK_ENTRY(regs[11]), g_strdup_printf("%u", (reg_vals[3]&0xff00)>>8));
	gtk_entry_set_text(GTK_ENTRY(regs[12]), g_strdup_printf("%u", (reg_vals[0]&0xff)));
	gtk_entry_set_text(GTK_ENTRY(regs[13]), g_strdup_printf("%u", (reg_vals[1]&0xff)));
	gtk_entry_set_text(GTK_ENTRY(regs[14]), g_strdup_printf("%u", (reg_vals[2]&0xff)));
	gtk_entry_set_text(GTK_ENTRY(regs[15]), g_strdup_printf("%u", (reg_vals[3]&0xff)));
	
}

void gui_init(void)
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window,500,400);
	
	GtkWidget *table = gtk_table_new(3, 2, FALSE);
	
	GtkWidget *button_box, *run, *step, *stop;

	notebook = gtk_notebook_new();
	
	regs_table = gtk_table_new(8, 6, FALSE);
	
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("AX"),0,1,0,1,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("BX"),0,1,1,2,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("CX"),0,1,2,3,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("DX"),0,1,3,4,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("DI"),0,1,4,5,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("SI"),0,1,5,6,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("BP"),0,1,6,7,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("SP"),0,1,7,8,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("AH"),2,3,0,1,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("BH"),2,3,1,2,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("CH"),2,3,2,3,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("DH"),2,3,3,4,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("AL"),4,5,0,1,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("BL"),4,5,1,2,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("CL"),4,5,2,3,0,0,0,0);
	gtk_table_attach(GTK_TABLE(regs_table),gtk_label_new("DL"),4,5,3,4,0,0,0,0);
	
	GtkWidget *regs_align = gtk_alignment_new(0,0,1,1);
	gtk_container_add(GTK_CONTAINER(regs_align), regs_table);
	
	int i = 0;
	for(i = 0;i<8;i++)
	{
		regs[i] = gtk_entry_new();
		gtk_editable_set_editable(GTK_EDITABLE(regs[i]),FALSE);
		gtk_table_attach(GTK_TABLE(regs_table),regs[i],1,2,i,i+1,0,GTK_FILL|GTK_EXPAND,0,0);
		gtk_widget_set_size_request(regs[i],64,20);
	}
	
	for(i = 8;i<12;i++)
	{
		regs[i] = gtk_entry_new();
		gtk_editable_set_editable(GTK_EDITABLE(regs[i]),FALSE);
		gtk_table_attach(GTK_TABLE(regs_table),regs[i],3,4,i-8,i-8+1,0,GTK_FILL|GTK_EXPAND,0,0);
		gtk_widget_set_size_request(regs[i],64,20);
	}
	for(i = 12;i<16;i++)
	{
		regs[i] = gtk_entry_new();
		gtk_editable_set_editable(GTK_EDITABLE(regs[i]),FALSE);
		gtk_table_attach(GTK_TABLE(regs_table),regs[i],5,6,i-12,i-12+1,0,GTK_FILL|GTK_EXPAND,0,0);
		gtk_widget_set_size_request(regs[i],64,20);
	}
	
	gui_var_list_create();
	
	/* Notebook Pages */
	
	code_view = gtk_text_view_new();
	gui_code_list_create();
	stdout_tv = gtk_text_view_new();
	
	/* Use a monospace font for text */
	gtk_widget_modify_font(code_view,pango_font_description_from_string("courier 10"));
	gtk_widget_modify_font(code_list,pango_font_description_from_string("courier 10"));
	gtk_widget_modify_font(stdout_tv,pango_font_description_from_string("courier 10"));
	
	
	/* Notebook page adapters - scrollbars */
	
	GtkWidget *scroll_windows[4];
	for(i = 0;i<4;i++)
	{
		scroll_windows[i] = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_windows[i]),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll_windows[i]),GTK_SHADOW_IN);
	}
	
	gtk_container_add(GTK_CONTAINER(scroll_windows[0]),code_view);
	gtk_container_add(GTK_CONTAINER(scroll_windows[1]),code_list);
	gtk_container_add(GTK_CONTAINER(scroll_windows[2]),stdout_tv);
	gtk_container_add(GTK_CONTAINER(scroll_windows[3]),var_list);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),scroll_windows[0],gtk_label_new("Code"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),scroll_windows[1],gtk_label_new("Code Trace"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),scroll_windows[2],gtk_label_new("Stdout"));
	
	/* Add the Variable List with the scrollbars now! */
	gtk_table_attach(GTK_TABLE(regs_table),scroll_windows[3],2,6,4,8,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
	
	/* Run and Step Buttons */
	
	run = gtk_button_new_with_label("Run");
	stop = gtk_button_new_with_label("Stop");
	step = gtk_button_new_with_label("Step");
	
	button_box = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box),GTK_BUTTONBOX_EDGE);
	gtk_box_pack_start(GTK_BOX(button_box),run, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(button_box),stop, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(button_box),step, FALSE, FALSE, 0);
	
	gtk_table_attach(GTK_TABLE(table), notebook, 0,1,1,2,GTK_EXPAND|GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),regs_align,1,2,1,2,GTK_EXPAND|GTK_FILL,GTK_EXPAND|GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),button_box,0,2,2,3,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);

	gtk_container_add(GTK_CONTAINER(window), table);
	
	/*
		Callback connections
	*/
	
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(window_delete_event),NULL);
	g_signal_connect(G_OBJECT(run),"clicked", G_CALLBACK(run_clicked), NULL);
	g_signal_connect(G_OBJECT(stop),"clicked", G_CALLBACK(stop_clicked), NULL);		
	gtk_widget_show_all(window);
}
