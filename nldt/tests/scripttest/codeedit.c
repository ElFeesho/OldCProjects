#include <gtk/gtk.h>
#include <sys/stat.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcelanguagesmanager.h>

static void ce_api_tree_row(GtkWidget *api_tree, GtkTreePath *path, GtkTreeViewColumn *col, GtkWidget *editor)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(api_tree));
	gtk_tree_model_get_iter(model, &iter, path);
	
	char *code = NULL;
	gtk_tree_model_get(model, &iter, 1, &code, -1);
	
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
	gtk_text_buffer_insert_at_cursor(buffer,code,-1);
	gtk_widget_grab_focus(editor);
}

static gboolean ce_destroy(GtkWidget *window, gpointer data)
{
	return FALSE;
}

static void ce_save(GtkWidget *button, GtkWidget *editor)
{
	char *filename = g_strdup(g_object_get_data(G_OBJECT(editor),"filename"));
	
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(buffer,&start,&end);
	char *file_text = gtk_text_buffer_get_text(buffer,&start,&end,TRUE);
	int size = gtk_text_iter_get_offset(&end);
	
	FILE *sfile = fopen(filename,"wb");
	if(size>0) 
	{
		fwrite(file_text,1,size,sfile);
	}
	fclose(sfile);

	g_object_set_data(G_OBJECT(editor),"modified", (void*)FALSE);
	g_free(file_text);
	g_free(filename);
}

static GtkWidget *ce_create_api_tree()
{
	GtkWidget *api_tree = gtk_tree_view_new();
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(api_tree),FALSE);
	GtkTreeViewColumn *api_col = gtk_tree_view_column_new();
	
	GtkTreeStore *api_model = gtk_tree_store_new(2,G_TYPE_STRING,G_TYPE_STRING);
	GtkTreeIter row, child, subchild;
	gtk_tree_store_append(api_model,&row,NULL);
	/*
		Draw API Funcs
	*/
	gtk_tree_store_set(api_model,&row,0,"Drawing (draw)",1,"draw.METHOD()",-1);
	gtk_tree_store_append(api_model,&child,&row);
	gtk_tree_store_set(api_model,&child,0,"box_rel [nil]",1,"draw.box_rel(x, y, w, h, outline_colour, fill_colour [, destination ])",-1);
	gtk_tree_store_append(api_model,&child,&row);
	gtk_tree_store_set(api_model,&child,0,"line_rel [nil]",1,"draw.line_rel(x, y, x2, y2, width, line_colour)",-1);
	
	/*
		GTK Api Funcs
	*/
	
	gtk_tree_store_append(api_model,&row,0);
	gtk_tree_store_set(api_model,&row,0,"GTK (gtk)",1,"gtk.METHOD()",-1);
	gtk_tree_store_append(api_model,&child,&row);
	gtk_tree_store_set(api_model,&child,0,"widget",1,"gtk.widget_",-1);
	gtk_tree_store_append(api_model,&subchild,&child);
	gtk_tree_store_set(api_model,&subchild,0,"show [nil]",1,"gtk.show(widget_id)",-1);
	gtk_tree_store_append(api_model,&subchild,&child);
	gtk_tree_store_set(api_model,&subchild,0,"hide [nil]",1,"gtk.hide(widget_id)",-1);
	gtk_tree_store_append(api_model,&subchild,&child);
	gtk_tree_store_set(api_model,&subchild,0,"set_position [nil]",1,"gtk.set_position(x, y [, w, h ])",-1);
	gtk_tree_store_append(api_model,&subchild,&child);
	gtk_tree_store_set(api_model,&subchild,0,"signal_connect [nil]",1,"gtk.signal_connect(widget_id, signal, callback)",-1);
	
	gtk_tree_store_append(api_model,&child,&row);
	gtk_tree_store_set(api_model,&child,0,"button",1,"gtk.button_",-1);
	gtk_tree_store_append(api_model,&subchild,&child);
	gtk_tree_store_set(api_model,&subchild,0,"button_new",1,"gtk.button_new(label, x, y [, w, h ])",-1);
	gtk_tree_store_append(api_model,&subchild,&child);
	gtk_tree_store_set(api_model,&subchild,0,"button_set_label",1,"gtk.button_set_label(widget_id, label)",-1);

	gtk_tree_store_append(api_model,&child,&row);
	gtk_tree_store_set(api_model,&child,0,"label",1,"gtk.label_",-1);
	gtk_tree_store_append(api_model,&subchild,&child);
	gtk_tree_store_set(api_model,&subchild,0,"label_new [number]",1,"gtk.label_new(label [, width,height])",-1);
	gtk_tree_store_append(api_model,&subchild,&child);
	gtk_tree_store_set(api_model,&subchild,0,"label_set_text [number]",1,"gtk.label_text(label)",-1);
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(api_tree), GTK_TREE_MODEL(api_model));
	
	GtkCellRenderer *text_rend = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(api_col, text_rend, TRUE);
	gtk_tree_view_column_add_attribute(api_col, text_rend, "text", 0);
	gtk_tree_view_append_column(GTK_TREE_VIEW(api_tree), api_col);
	return api_tree;
}

static void ce_init_editor(GtkWidget *source_view, char *file_name)
{
	GtkSourceLanguagesManager *lang_man = gtk_source_languages_manager_new();
	GtkSourceBuffer *src_buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(source_view)));
	GtkSourceLanguage *lang = gtk_source_languages_manager_get_language_from_mime_type(lang_man,"text/x-lua");
	if(lang)
		gtk_source_buffer_set_language(src_buf, lang);
	else
		fprintf(stderr,"Couldn't find lua highlighting file for GtkSourceView\n");
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(source_view),TRUE);
	gtk_source_view_set_tabs_width(GTK_SOURCE_VIEW(source_view),3);
	gtk_source_buffer_set_highlight(src_buf,TRUE);
	PangoFontDescription *pfont_desc = pango_font_description_from_string("courier new 10");
   gtk_widget_modify_font(source_view, pfont_desc);
	
	if(file_name != NULL)
	{
		GtkTextBuffer *txt_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(source_view));
		GtkTextIter s_iter;
		gtk_text_buffer_get_start_iter(txt_buf,&s_iter);
		struct stat file_stat;
		if(stat(file_name,&file_stat)!=0)
			return;
			
		FILE *lua_file = fopen(file_name,"r");
		char *file_buffer = g_malloc0(file_stat.st_size);
		fread(file_buffer,1,file_stat.st_size,lua_file);
		gtk_text_buffer_insert(txt_buf,&s_iter,file_buffer,file_stat.st_size);
		fclose(lua_file);
		g_free(file_buffer);
	}
}

static GtkWidget *ce_create(char *file_name)
{
	GtkWidget *ce_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(ce_window),700,500);
	
	GtkWidget *vbox = gtk_vbox_new(0,10);
	
	GtkWidget *toolbar = gtk_toolbar_new();
	GtkToolItem *save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(save),-1);
	
	GtkWidget *hbox = gtk_hpaned_new();
	
	GtkWidget *api_scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(api_scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(api_scroll),GTK_SHADOW_IN);
	GtkWidget *api_tree = ce_create_api_tree();
	gtk_container_add(GTK_CONTAINER(api_scroll),api_tree);
	gtk_widget_set_size_request(api_scroll,30,-1);
	GtkWidget *editor_scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(editor_scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(editor_scroll),GTK_SHADOW_IN);
	GtkWidget *editor = gtk_source_view_new();
	ce_init_editor(editor, file_name);
	gtk_container_add(GTK_CONTAINER(editor_scroll),editor);

	gtk_container_add(GTK_CONTAINER(ce_window),vbox);
	gtk_box_pack_start(GTK_BOX(vbox),toolbar,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);
	gtk_paned_pack1(GTK_PANED(hbox),api_scroll,TRUE,TRUE);
	gtk_paned_pack2(GTK_PANED(hbox),editor_scroll,TRUE,TRUE);
	
	g_signal_connect(G_OBJECT(api_tree),"row-activated",G_CALLBACK(ce_api_tree_row),editor);
	
	g_signal_connect(G_OBJECT(ce_window),"delete-event",G_CALLBACK(ce_destroy),NULL);
	
	g_signal_connect(G_OBJECT(save),"clicked",G_CALLBACK(ce_save),editor);
	
	g_object_set_data(G_OBJECT(editor),"filename",file_name);
	
	return ce_window;
}

extern void ce_show(char *file_name)
{
	GtkWidget *window = ce_create(file_name);
	gtk_widget_show_all(window);
}
