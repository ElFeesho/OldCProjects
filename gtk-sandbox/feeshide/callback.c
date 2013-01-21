/*

Feeshide- The Coders' Element: Small lightweight integrated development environment
Copyright (C) 2006  Christopher (Feesh!) Sawczuk

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include <libxml/xmlreader.h>
#include "callback.h"
#include "data.xbm"

int pages = 1; // Set page count to 1, resulting in "Untitled 1" as first new page

void notebookTabPopup(GtkWidget *my_widget, GdkEventButton *event)
{
	//Show a popup menu on tabs
	GtkWidget *menu;
	int button, event_time;
	menu = gtk_menu_new ();
	GtkWidget *menuItem = gtk_menu_item_new_with_label("Save");
	gtk_menu_append(GTK_MENU(menu),menuItem);
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuSave),NULL);
	menuItem = gtk_menu_item_new_with_label("Save As");
	gtk_menu_append(GTK_MENU(menu),menuItem);
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuSaveAs),NULL);
	menuItem = gtk_menu_item_new_with_label("Add to Group");
	gtk_menu_append(GTK_MENU(menu),menuItem);
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuAddToGroup),NULL);
	menuItem = gtk_menu_item_new_with_label("Close");
	gtk_menu_append(GTK_MENU(menu),menuItem);
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuPageClose),NULL);
	gtk_widget_show_all(menu);
	if(event)
	{
		button = event->button;
		event_time = event->time;
	}
	else
	{
		button = 0;
		event_time = gtk_get_current_event_time ();
	}
	gtk_menu_attach_to_widget (GTK_MENU (menu), my_widget, NULL);
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, button, event_time);
}

void groupPopupMenu(GtkWidget *my_widget, GdkEventButton *event)
{
	//Show a popup menu on group items
	GtkWidget *menu;
	int button, event_time;
	menu = gtk_menu_new ();
	GtkWidget *menuItem = gtk_menu_item_new_with_label("Remove");
	gtk_menu_append(GTK_MENU(menu),menuItem);
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(groupRemoveFile),NULL);
	gtk_widget_show_all(menu);
	if(event)
	{
		button = event->button;
		event_time = event->time;
	}
	else
	{
		button = 0;
		event_time = gtk_get_current_event_time ();
	}
	gtk_menu_attach_to_widget (GTK_MENU (menu), my_widget, NULL);
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, button, event_time);
}

gboolean onTreeViewMouse(GtkWidget *widget, GdkEventButton *event)
{
	//Make right clicks (mouse 3) launch context menu
	if (event->button == 3 && event->type == GDK_BUTTON_PRESS)
	{
		groupPopupMenu(widget, event);
		return 1;
	}
	return 0;
}

gboolean onNoteMouse(GtkWidget *widget, GdkEventButton *event)
{
	//Make right clicks (mouse 3) launch context menu
	if (event->button == 3 && event->type == GDK_BUTTON_PRESS)
	{
		notebookTabPopup(widget, event);
		return 1;
	}
	return 0;
}

static void errorDialog(char *msg)
{
	//Launch an error dialog, going to be moved into a helper function file
	GtkWidget *dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,msg,0);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void setGroupName(char *group)
{
	//Change the groups name to group, only used right now to clear the group bar
	GtkTreeIter iter;
	mainGUI.cGroup=group;
	mainGUI.treeStore = gtk_list_store_new(3,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_UINT);
	gtk_list_store_clear(mainGUI.treeStore);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (mainGUI.treeStore), 0, GTK_SORT_ASCENDING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(mainGUI.treeView),GTK_TREE_MODEL(mainGUI.treeStore));
}

void onMenuNewGroup(GtkWidget *widget, gpointer userdata)
{
	//Create a new group
	GtkFileChooserDialog *dialog = (GtkFileChooserDialog*)gtk_file_chooser_dialog_new("New Group File",GTK_WINDOW(mainGUI.window),GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),"NewGroup.fig");
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
	int res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		char *foldername = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER (dialog));
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		FILE *file = fopen(filename,"w");
		if(file)
		{
			fprintf(file,"<!-- Generated by Feeshide: FEEL FREE TO ALTER -->");
			fprintf(file,"<group>\n");
			fprintf(file,"\t<!-- Files Here -->\n");
			fprintf(file,"</group>");
			fclose(file);
			setGroupName(filename);
		}else{
			errorDialog("Couldn't create group file!");
			return;
		}
	}else{
		gtk_widget_destroy(GTK_WIDGET(dialog));
		return;
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return;
}

void onMenuOpenGroup(GtkWidget *widget, gpointer userdata)
{
	//Open a group after checking if the previous group should be saved or if the operation should be cancelled
	if(mainGUI.cGroup!=0)
		;//ask if wanna change
	
	GtkFileChooserDialog *dialog = (GtkFileChooserDialog*)gtk_file_chooser_dialog_new("Open Group File",GTK_WINDOW(mainGUI.window),GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	int res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT)
	{
		char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
		char *foldername = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER (dialog));
		xmlTextReaderPtr reader;
		reader = xmlReaderForFile(filename, NULL, 0);
		if(reader)
		{
			setGroupName(filename);
			int ret;
			const xmlChar *name, *ftitle;
			while((ret = xmlTextReaderRead(reader))==1)
			{
    			name = xmlTextReaderConstName(reader);
				printf("Name: %s\n",name);
				if(strcmp(name,"file")==0)
				{
					name = xmlTextReaderGetAttribute(reader,"name");
					ftitle = xmlTextReaderGetAttribute(reader,"title");
					printf("Title: %s\n",ftitle);
					FILE *nFile = fopen(name,"r");
					if(nFile)
					{
						int id = newNotePage((char*)ftitle);
						GtkTreeIter iter;
						gtk_list_store_append(mainGUI.treeStore,&iter);
						gtk_list_store_set(mainGUI.treeStore,&iter,0,ftitle,1,name,2,id,-1);
						GtkSourceBuffer *buff = fileGetBuffer(id);
						GtkTextIter end;
						char *data = malloc(256);
						memset(data,'\0',256);
						while(fread(data,1,255,nFile)!=0) //Don't read past 255 and remove the \0!
						{
							gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buff),&end);
							gtk_text_buffer_insert(GTK_TEXT_BUFFER(buff),&end,data,-1);
							memset(data,'\0',256);
						}
						highlightBuffer(buff,(char*)name);
						fileSetSaved(id,1);
						GtkWidget *label = fileGetLabel(id);
						gtk_label_set_text(GTK_LABEL(label),(char*)ftitle);
					}
				}
			}
			xmlTextReaderClose(reader);
		}
		else
		{
				errorDialog("Couldn't read file");
		}
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void onMenuSaveGroup(GtkWidget *widget, gpointer userdata)
{
	//Save the current group open as long as there is a group open
	if(mainGUI.cGroup!=0)
	{
		GtkTreeIter iter;
		if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mainGUI.treeStore),&iter))
		{
			FILE *grpFile = fopen(mainGUI.cGroup,"w");
			fprintf(grpFile,"<!-- Generated by Feeshide: FEEL FREE TO ALTER -->");
			fprintf(grpFile,"<group>\n");
			char valid = 1;
			while(valid)
			{
				gchar *title;
				gchar *name;
				gtk_tree_model_get(GTK_TREE_MODEL(mainGUI.treeStore), &iter, 0, &title, -1);
				gtk_tree_model_get(GTK_TREE_MODEL(mainGUI.treeStore), &iter, 1, &name, -1);
				fprintf(grpFile,"\t<file name=\"%s\" title=\"%s\" />\n",name,title);
				valid = gtk_tree_model_iter_next (GTK_TREE_MODEL(mainGUI.treeStore), &iter);
				printf("Writing group\n");
			}
			fprintf(grpFile,"</group>");
			fclose(grpFile);
		}else{
			errorDialog("No files in group!");
			return;
		}
	}
	else
	{
		onMenuNewGroup(NULL,NULL);
		onMenuSaveGroup(NULL,NULL);
	}
}

void onMenuNewFileGroup(GtkWidget *widget, gpointer userdata)
{
	//Create a new file to add to the group, file at current must exist.
	GtkFileChooserDialog *dialog = (GtkFileChooserDialog*)gtk_file_chooser_dialog_new("New File into Group",GTK_WINDOW(mainGUI.window),GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
	int res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		char *foldername = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER (dialog));
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		FILE *file = fopen(filename,"w");
		int id;
		if(file)
		{
			id = newNotePage(filename);
			GtkTreeIter iter;
			gtk_list_store_append(mainGUI.treeStore,&iter);
			gtk_list_store_set(mainGUI.treeStore,&iter,0,filename+strlen(foldername)+1,1,filename,2,id,-1);
			fclose(file);
		}else{
			errorDialog("Couldn't create file to add to group!");
			return;
		}
		highlightBuffer(fileGetBuffer(id),filename);
		fileSetSaved(id,1);
		fileSetFileName(id, filename);
		GtkWidget *label = fileGetLabel(id);
		gtk_label_set_text(GTK_LABEL(label),filename+strlen(foldername)+1);
	}else{
		gtk_widget_destroy(GTK_WIDGET(dialog));
		return;
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return;
}
void groupRemoveFile(GtkWidget *widget, gpointer user_data)
{
	//Removes the file from the group, removing it from compilations on compilable languages
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL(mainGUI.treeStore);
	GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(mainGUI.treeView));
	gtk_tree_selection_get_selected(sel,&model,&iter);
	gtk_list_store_remove(mainGUI.treeStore,&iter);
}

void onMenuAddFileGroup(GtkWidget *widget, gpointer userdata)
{
	//Add a pre-existing file to the group
	GtkFileChooserDialog *dialog = (GtkFileChooserDialog*)gtk_file_chooser_dialog_new("Open File",GTK_WINDOW(mainGUI.window),GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	int res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT)
	{
		char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
		char *foldername = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER (dialog));
		FILE *nFile = fopen(filename,"r");
		if(nFile)
		{
			int id = newNotePage(filename);
			GtkTreeIter iter;
			gtk_list_store_append(mainGUI.treeStore,&iter);
			gtk_list_store_set(mainGUI.treeStore,&iter,0,filename+strlen(foldername)+1,1,filename,2,id,-1);
			GtkSourceBuffer *buff = fileGetBuffer(id);
			GtkTextIter end;
			char *data = malloc(256);
			memset(data,'\0',256);
			while(fread(data,1,255,nFile)!=0) //Don't read past 255 and remove the \0!
			{
				gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buff),&end);
				gtk_text_buffer_insert(GTK_TEXT_BUFFER(buff),&end,data,-1);
				memset(data,'\0',256);
			}
			highlightBuffer(buff,filename);
			fileSetSaved(id,1);
			GtkWidget *label = fileGetLabel(id);
			gtk_label_set_text(GTK_LABEL(label),filename+(strlen(foldername)+1));
		}
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}


void onMenuAddToGroup(GtkWidget *widget, gpointer userdata)
{
	//For right click context menu on tabs, if a user adds the file to the group
	int id;
	fileList *temp = filelist;
	while(temp)
	{
		if(temp->pageNum == gtk_notebook_get_current_page(GTK_NOTEBOOK(mainGUI.notebook)))
		{
			id = temp->id;
		}
		temp = temp->next;
	}
	GtkTreeIter iter;
	const char *title = gtk_label_get_text(GTK_LABEL(fileGetLabelByPage(gtk_notebook_get_current_page(GTK_NOTEBOOK(mainGUI.notebook)))));
	gtk_list_store_append(mainGUI.treeStore,&iter);
	gtk_list_store_set(mainGUI.treeStore,&iter,0,title,1,fileGetFileNameByPage(gtk_notebook_get_current_page(GTK_NOTEBOOK(mainGUI.notebook))),2,id,-1);
}


void onGroupActivate(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	//When a group file is double clicked, in the group bar, go to it or reopen it
	GtkTreeIter iter;
	gtk_tree_model_get_iter(GTK_TREE_MODEL(mainGUI.treeStore),&iter, path);
	gchar *title;
	gchar *name;
	gint *idf;
	gtk_tree_model_get(GTK_TREE_MODEL(mainGUI.treeStore), &iter, 0, &title, -1);
	gtk_tree_model_get(GTK_TREE_MODEL(mainGUI.treeStore), &iter, 1, &name, -1);
	gtk_tree_model_get(GTK_TREE_MODEL(mainGUI.treeStore), &iter, 2, &idf, -1);
	fileList *temp = filelist;
	char found = 0;
	while(temp!=NULL)
	{
		if(temp->id == (int)idf)
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(mainGUI.notebook),temp->pageNum);
			found = 1;
			break;
		}
		temp=temp->next;
	}
	if(!found)
	{
		//Reopen the closed tab
		FILE *nFile = fopen(name,"r");
		if(nFile)
		{
			int id = newNotePage(title);
			gtk_list_store_set(mainGUI.treeStore, &iter, 2, id, -1);
			GtkSourceBuffer *buff = fileGetBuffer(id);
			fileSetFileName(id,name);
			highlightBuffer(buff,name);
			GtkWidget *label = fileGetLabel(id);
			gtk_label_set_text(GTK_LABEL(label),title);
			GtkTextIter end;
			char *data = malloc(256);
			memset(data,'\0',256);
			while(fread(data,1,255,nFile)!=0) //Don't read past 255 and remove the \0!
			{
				gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buff),&end);
				gtk_text_buffer_insert(GTK_TEXT_BUFFER(buff),&end,data,-1);
				memset(data,'\0',256);
			}
			fileSetSaved(id,1);
			fclose(nFile);
		}else{
			errorDialog("File is no longer readable!");
		}
	}
}

GtkWidget *newPage(GtkSourceBuffer *buf)
{
	//Create a new page, used in conjunction with newNotebookPage
	GtkWidget *scroll = gtk_scrolled_window_new(NULL,NULL);	
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	GtkWidget *textv = gtk_source_view_new();
//	g_object_set_data_full(G_OBJECT (buf), "textView", textv, (GDestroyNotify) g_free);
	g_object_set_data_full(G_OBJECT (buf), "textView", textv, 0);
	PangoFontDescription* p = pango_font_description_from_string("courier new 10");
	gtk_widget_modify_font(textv,p);
	gtk_source_buffer_set_highlight(buf,TRUE);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(textv),GTK_TEXT_BUFFER(buf));
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(textv), TRUE);
	gtk_source_view_set_tabs_width(GTK_SOURCE_VIEW(textv),3);
	gtk_container_add(GTK_CONTAINER(scroll),textv);
	return scroll;
}

void onMenuFont(GtkWidget *widget, gpointer userdata)
{
	//Font changing dialog will eventually save its settings to the config file
	GtkWidget *fontSel = gtk_font_selection_dialog_new("Choose a Font");
	gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(fontSel),mainGUI.font);
	int res = gtk_dialog_run(GTK_DIALOG(fontSel));
	if(res == GTK_RESPONSE_ACCEPT)
	{
		char *font = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(fontSel));
		GtkSourceBuffer *buf = fileGetBufferByPage(gtk_notebook_get_current_page(GTK_NOTEBOOK(mainGUI.notebook)));
		GtkWidget *view = g_object_get_data (G_OBJECT (buf), "textView");
		strcpy(mainGUI.font,font);
		PangoFontDescription* p = pango_font_description_from_string(font);
		gtk_widget_modify_font(view,p);
		free(view);
		free(font);
		free(p);
	}
	gtk_widget_destroy(fontSel);
}

void onMenuToolbar(GtkWidget *widget, gpointer userdata)
{
	//Hide the toolbar
	if(!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
		gtk_widget_hide(mainGUI.toolbar);
	else
		gtk_widget_show(mainGUI.toolbar);
}

void onMenuPageFirst(GtkWidget *widget, gpointer userdata)
{
	//Go to the notebook's first page
	gtk_notebook_set_current_page(GTK_NOTEBOOK(mainGUI.notebook),0);
}

void onMenuPageLast(GtkWidget *widget, gpointer userdata)
{
	//Go to the notebook's last page
	gtk_notebook_set_current_page(GTK_NOTEBOOK(mainGUI.notebook),-1);
}

void onMenuAbout(GtkWidget *widget, gpointer userdata)
{
	//Plaster a big ol Feesh on the screen, fish courtesy of Gwildor :D
	GtkWidget *window; 
	GtkRcStyle *rc = gtk_rc_style_new();
	rc->bg_pixmap_name[0]="back.xpm";
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window),TRUE);
	gtk_widget_modify_style(window,rc);
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER_ALWAYS);
	GdkBitmap *winMask = gdk_bitmap_create_from_data(NULL,data_bits,data_width,data_height);
	gtk_widget_set_size_request(window,data_width,data_height);
	gtk_window_set_decorated(GTK_WINDOW(window),0);
	gtk_widget_shape_combine_mask(window,winMask,0,0);
	g_signal_connect(G_OBJECT(window),"button-press-event",G_CALLBACK(gtk_widget_destroy),NULL);
	//Make the window catch button presses
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_BUTTON_PRESS_MASK);
	gtk_widget_show  (window);
}

int newNotePage(char *name)
{
	//Create a notebook page and fileList node
	int id = fileAdd(name);
	int pageNum = gtk_notebook_append_page(GTK_NOTEBOOK(mainGUI.notebook),newPage(fileGetBuffer(id)),fileGetLabel(id));
	fileSetPage(id,pageNum);
	gtk_widget_show_all(mainGUI.window);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(mainGUI.notebook),pageNum);
	gtk_widget_queue_draw(mainGUI.notebook);
	g_signal_connect(G_OBJECT(fileGetBuffer(id)),"changed",G_CALLBACK(onChange),(gpointer)pageNum);
	return id;
}

void onNoteMenu(GtkWidget *widget, gpointer userdata)
{
	//Show context menu on tabs, for saving, saving as, closing, and adding to groups
	notebookTabPopup(widget,0);
}

void onChange(GtkWidget *widget, gpointer user_data)
{
	//Update whether a file has been saved or not
	if(fileGetSavedByPage((int)user_data)==1)
		fileSetSavedByPage((int)user_data,0);
	if(fileGetSavedByPage((int)user_data)==-2)
		fileSetSavedByPage((int)user_data,-1);
}

gboolean onClose(GtkWidget *widget, gpointer user_data)
{
	//Do checks for whether a file is open and whether it has been saved
	char found = 0;
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->saved != 1 && temp->saved != -2)
		{
			found = 1;
		}
		temp = temp->next;
	}
	if(found)
	{
		GtkWidget *dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,0,"There is unsaved word. Do you wish to save?",0);
		gtk_dialog_add_buttons(GTK_DIALOG(dialog),"Yes",GTK_RESPONSE_ACCEPT,"No",GTK_RESPONSE_NO,"Cancel",GTK_RESPONSE_CANCEL,0);
		gint res = gtk_dialog_run(GTK_DIALOG(dialog));
		if(res == GTK_RESPONSE_ACCEPT)
		{
			temp=filelist;
			while(temp!=NULL)
			{
				if(temp->saved==0)
				{
					saveFile(temp->pageNum);
				}
				if(temp->saved==-1)
				{
					if(saveAs(temp->pageNum)!=0)
					{
						gtk_widget_destroy(dialog);
						return 1;
					}
				}
				temp=temp->next;
			}		
		}else if(res == GTK_RESPONSE_NO)
		{
			
		}else{
			gtk_widget_destroy(dialog);
			return 1;
		}
		gtk_widget_destroy(dialog);
	}
	gtk_exit(0);
}

void onMenuNew(GtkWidget *widget, gpointer userdata)
{
	//Create a new tab with Untitled N as the label
	char title[16];
	sprintf(title,"Untitled %d",pages++);
	newNotePage(title);
}

void highlightBuffer(GtkSourceBuffer *buff, char *filename)
{
	//Since windows doesn't have mimetypes going... a little hack to choose highlighting
	//dependent on file type. Mimetype matching will be implemented when I can test it.
	GtkSourceLanguagesManager *langM = gtk_source_languages_manager_new();
	GtkSourceLanguage *lang = 0;
}

void onMenuOpen(GtkWidget *widget, gpointer userdata)
{
	//Open up a file dialog for a file to load in.
	GtkFileChooserDialog *dialog = (GtkFileChooserDialog*)gtk_file_chooser_dialog_new("Open File",GTK_WINDOW(mainGUI.window),GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	int res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT)
	{
		char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
		char *foldername = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER (dialog));
		FILE *nFile = fopen(filename,"r");
		if(nFile)
		{
			int id = newNotePage(filename);
			GtkSourceBuffer *buff = fileGetBuffer(id);
			GtkTextIter end;
			char *data = malloc(256);
			memset(data,'\0',256);
			while(fread(data,1,255,nFile)!=0) //Don't read past 255 and remove the \0!
			{
				gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buff),&end);
				gtk_text_buffer_insert(GTK_TEXT_BUFFER(buff),&end,data,-1);
				memset(data,'\0',256);
			}
			highlightBuffer(buff,filename);
			fileSetSaved(id,1);
			GtkWidget *label = fileGetLabel(id);
			gtk_label_set_text(GTK_LABEL(label),filename+(strlen(foldername)+1));
		}
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void saveFile(int page)
{
	//Check whether a file can be saved straight off, or if it needs to be created
	GtkSourceBuffer *buff = fileGetBufferByPage(page);
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buff),&start, &end);
	char *data = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buff),&start, &end, 0);
	char *filename;
	filename = fileGetFileNameByPage(page);
	FILE *file = fopen(filename,"w");
	if(file)
	{
		fwrite(data,1,strlen(data),file);
		fclose(file);
		fileSetSavedByPage(page,1);
	}else{
		errorDialog("Couldn't save file!\n");
		return;
	}
	highlightBuffer(buff,filename);
}

char saveAs(int page)
{
	//Save As will always show a file dialog for where to save the new file
	GtkFileChooserDialog *dialog = (GtkFileChooserDialog*)gtk_file_chooser_dialog_new("Save File",GTK_WINDOW(mainGUI.window),GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	if(fileGetSavedByPage(page)>-1)
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog),fileGetFileNameByPage(page));
	else
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),fileGetFileNameByPage(page));
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
	int res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		char *foldername = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER (dialog));
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		GtkSourceBuffer *buff = fileGetBufferByPage(page);
		GtkTextIter start, end;
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buff),&start, &end);
		char *data = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buff),&start, &end, 0);
		FILE *file = fopen(filename,"w");
		if(file)
		{
			fwrite(data,1,strlen(data),file);
			fclose(file);
		}else{
			errorDialog("Couldn't save file!");
			return 1;
		}
		highlightBuffer(buff,filename);
		fileSetSavedByPage(page,1);
		fileSetFileNameByPage(page, filename);
		GtkWidget *label = fileGetLabelByPage(page);
		gtk_label_set_text(GTK_LABEL(label),filename+strlen(foldername)+1);
	}else{
		gtk_widget_destroy(GTK_WIDGET(dialog));
		return -1;
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return 0;
}

void onMenuGroupbar(GtkWidget *widget, gpointer userdata)
{
	//Hide the group bar, will possibly be removed
	if(!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
		gtk_widget_hide(mainGUI.treeView);
	else
		gtk_widget_show(mainGUI.treeView);
}

void onMenuSave(GtkWidget *widget, gpointer userdata)
{
	//Launch the saveFile function dependent on which tab is being viewed
	int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(mainGUI.notebook));
	if(fileGetSavedByPage(page)==-1)
	{
		onMenuSaveAs(NULL,NULL);
		return;
	}else{
		saveFile(page);
	}
}

void onMenuSaveAs(GtkWidget *widget, gpointer userdata)
{
	//Save As the currently viewed file.
	saveAs(gtk_notebook_get_current_page(GTK_NOTEBOOK(mainGUI.notebook)));
}

void onMenuPageClose(GtkWidget *widget, gpointer userdata)
{
	//Close a page, but check if it has been saved first, if it hasn't been touched it will close silently
	int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(mainGUI.notebook));
	if(page == -1) return;
	if(fileGetSavedByPage(page)>-2 && fileGetSavedByPage(page)<1)
	{
		GtkWidget *dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,0,"The page you are closing has unsaved data. Do you wish to save?",0);		
		gtk_dialog_add_buttons(GTK_DIALOG(dialog),"Yes",GTK_RESPONSE_ACCEPT,"No",GTK_RESPONSE_NO,"Cancel",GTK_RESPONSE_CANCEL,0);
		gint res = gtk_dialog_run(GTK_DIALOG(dialog));
		if(res == GTK_RESPONSE_ACCEPT)
		{
			if(fileGetSavedByPage(page)==0)
			{
				saveFile(page);
			}
			if(fileGetSavedByPage(page)==-1)
			{
				if(saveAs(page)!=0)
				{
					gtk_widget_destroy(dialog);
					return;
				}
			}
			//Lower all files with a page num higher than this one by 1
			fileList *temp = filelist;
			fileDeleteByPage(page);
			while(temp!=NULL)
			{
				if(temp->pageNum>page)
				{
					temp->pageNum--;
				}
				temp=temp->next;
			}
		}
		if(res == GTK_RESPONSE_NO)
		{
			fileList *temp = filelist;
			fileDeleteByPage(page);			
			while(temp!=NULL)
			{
				if(temp->pageNum>page)
				{
					temp->pageNum--;
				}
				temp=temp->next;
			}
			fileDeleteByPage(page);
		}
		if(res == GTK_RESPONSE_CANCEL)
		{
			gtk_widget_destroy(dialog);
			return;
		}
		gtk_widget_destroy(dialog);
	}else{
		fileList *temp = filelist;
		fileDeleteByPage(page);
		while(temp!=NULL)
		{
			if(temp->pageNum>page)
			{
				temp->pageNum--;
			}
			temp=temp->next;
		}
	}
	gtk_notebook_remove_page(GTK_NOTEBOOK(mainGUI.notebook),page);
}

void onMenuPageNext(GtkWidget *widget, gpointer userdata)
{
	//Convenience function to move to the next tab
	gtk_notebook_next_page(GTK_NOTEBOOK(mainGUI.notebook));
}

void onMenuPagePrev(GtkWidget *widget, gpointer userdata)
{
	//Convenience function to move to previous tab
	gtk_notebook_prev_page(GTK_NOTEBOOK(mainGUI.notebook));
}

void onMenuExit(GtkWidget *widget, gpointer userdata)
{
	//Do checks for whether a file is open and whether it has been saved
	char found = 0;
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->saved != 1 && temp->saved != -2)
		{
			found = 1;
		}
		temp = temp->next;
	}
	if(found)
	{
		GtkWidget *dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,0,"There is unsaved word. Do you wish to save?",0);		gtk_dialog_add_buttons(GTK_DIALOG(dialog),"Yes",GTK_RESPONSE_ACCEPT,"No",GTK_RESPONSE_NO,"Cancel",GTK_RESPONSE_CANCEL,0);
		gint res = gtk_dialog_run(GTK_DIALOG(dialog));
		if(res == GTK_RESPONSE_ACCEPT)
		{
			temp=filelist;
			while(temp!=NULL)
			{
				if(temp->saved==0)
				{
					saveFile(temp->pageNum);
				}
				if(temp->saved==-1)
				{
					if(saveAs(temp->pageNum)!=0)
					{
						gtk_widget_destroy(dialog);
						return;
					}
				}
				temp=temp->next;
			}		
		}else if(res == GTK_RESPONSE_NO)
		{
			
		}else{
			gtk_widget_destroy(dialog);
			return;
		}
		gtk_widget_destroy(dialog);
	}
	gtk_exit(0);
}
