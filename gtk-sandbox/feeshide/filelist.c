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

#include "filelist.h"

int fileAdd(char *filename)
{
	//Create a fileList node in linklist
	static idPos = 0;
	if(!filelist)
	{
		filelist = malloc(sizeof(fileList));
		strcpy(filelist->fname,filename);
		filelist->id = idPos;
		filelist->saved = -2;
		filelist->pageNum = -1;
		filelist->label = gtk_label_new(filename);
		filelist->textBuff = gtk_source_buffer_new(NULL);
		filelist->next = 0;
		filelist->sourceView = 0;
		return idPos++;
	}else{
		fileList *temp = malloc(sizeof(fileList));
		strcpy(temp->fname,filename);
		temp->id = idPos;
		temp->saved = -2;
		temp->pageNum = -1;
		temp->label = gtk_label_new(filename);
		temp->textBuff = gtk_source_buffer_new(NULL);
		temp->next = filelist;
		temp->sourceView = 0;
		filelist = temp;
		return idPos++;
	}
}

GtkSourceBuffer *fileGetBuffer(gint id)
{
	//Retrieve a file's buffer by its ID
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->id == id)
		{
			return temp->textBuff;
		}
		temp=temp->next;
	}
	return 0;
}

GtkWidget *fileGetLabel(gint id)
{
	//Retrieve a file's label (used in notebook tab)
	fileList *temp = filelist;
	while(temp->id != id && temp->next !=NULL)
	{
		temp=temp->next;
	}
	if(temp->id == id)
		return temp->label;
	else
		return 0;
}

GtkSourceBuffer *fileGetBufferByPage(gint page)
{
	//Get the file's buffer by page
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->pageNum == page)
		{
			return temp->textBuff;
		}
		temp=temp->next;
	}
	return 0;
}

GtkWidget *fileGetLabelByPage(gint page)
{
	//Get the file's label by page
	fileList *temp = filelist;
	while(temp->pageNum != page && temp->next !=NULL)
	{
		temp=temp->next;
	}
	if(temp->pageNum == page)
		return temp->label;
	else
		return 0;
}

char *fileGetFileNameByPage(int page)
{
	//Get a file's name by page
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->pageNum == page)
		{
			return temp->fname;
		}
		temp=temp->next;
	}
	return 0;
}

void fileSetFileNameByPage(int page,char *nfile)
{
	//Set file name for the filelist node with page number page
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->pageNum == page)
		{
			strcpy(temp->fname,nfile);
			return;
		}
		temp=temp->next;
	}
}

void fileSetFileName(int id,char *nfile)
{
	//Set a filelist node's filename by id
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->id == id)
		{
			strcpy(temp->fname,nfile);
			return;
		}
		temp=temp->next;
	}
}

char fileGetSavedByPage(int page)
{
	//Get whether a file is saved by page
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->pageNum == page)
		{
			return temp->saved;
		}
		temp=temp->next;
	}
}

void fileSetSavedByPage(int page, char val)
{
	//Set whether a file is saved by page
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->pageNum == page)
		{
			temp->saved = val;
			return;
		}
		temp=temp->next;
	}
}

char fileGetSaved(int id)
{
	//Get whether a file is saved by id
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->id == id)
		{
			return temp->saved;
		}
		temp=temp->next;
	}
}

void fileSetSaved(int id, char val)
{
	//Set a file to be saved by Id
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->id == id)
		{
			temp->saved = val;
			return;
		}
		temp=temp->next;
	}
}

void fileSetPage(int id, int pageNum)
{
	//Set the file's page
	fileList *temp = filelist;
	while(temp!=NULL)
	{
		if(temp->id == id)
		{
			temp->pageNum = pageNum;
			return;
		}
		temp=temp->next;
	}	
}

void fileDeleteByPage(int pageNum)
{
	//Delete the files contents
	fileList *temp = filelist;
	fileList *prev = NULL;
	while(temp!=NULL)
	{
		if(temp->pageNum == pageNum)
		{
			if(prev!=NULL)
			{
				prev->next = temp->next;
				fileClear(&temp);
				free(temp);
				return;
			}else{
				filelist = temp->next;
				fileClear(&temp);
				free(temp);
				return;
			}
		}
		prev = temp;
		temp=temp->next;
	}
}

void fileClear(fileList **elem)
{
	//Helper function to free data used in the list
	//And to prevent any zombie nodes
	g_free((*elem)->textBuff);
	(*elem)->next = 0;
}
