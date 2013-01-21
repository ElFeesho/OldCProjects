#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "question.h"

struct quest_node *quest_list = NULL;

static void question_clear_all();

int question_load_questions(char *sql)
{
	question_clear_all();
	int count = 0;
	
	sqlite3_stmt *query = database_query(sql);
	while(sqlite3_step(query)==SQLITE_ROW)
	{
		count++;
		if(quest_list == NULL)
		{
			quest_list = malloc(sizeof(struct quest_node));
			quest_list->quest = malloc(sizeof(struct question));
			quest_list->quest->caption = g_strdup((char*)sqlite3_column_text(query,0));
			quest_list->quest->filename = g_strdup((char*)sqlite3_column_text(query,1));
			quest_list->quest->group = g_strdup((char*)sqlite3_column_text(query,2));
			quest_list->quest->level = g_strdup((char*)sqlite3_column_text(query,3));
			quest_list->quest->type = g_strdup((char*)sqlite3_column_text(query,4));
			quest_list->quest->topic = g_strdup((char*)sqlite3_column_text(query,5));
			quest_list->quest->stopic = g_strdup((char*)sqlite3_column_text(query,6));
			quest_list->next = NULL;
		}
		else
		{
			struct quest_node *temp = malloc(sizeof(struct quest_node));
			temp->quest = malloc(sizeof(struct question));
			temp->quest->caption = g_strdup((char*)sqlite3_column_text(query,0));
			temp->quest->filename = g_strdup((char*)sqlite3_column_text(query,1));
			temp->quest->group = g_strdup((char*)sqlite3_column_text(query,2));
			temp->quest->level = g_strdup((char*)sqlite3_column_text(query,3));
			temp->quest->type = g_strdup((char*)sqlite3_column_text(query,4));
			temp->quest->topic = g_strdup((char*)sqlite3_column_text(query,5));
			temp->quest->stopic = g_strdup((char*)sqlite3_column_text(query,6));
			temp->next = quest_list;
			quest_list = temp;
		}
	}
	return count;
}

int question_get_total()
{
	struct quest_node *temp = quest_list;
	int count = 0;
	while(temp)
	{
		count++;
		temp = temp->next;
	}
	return count;
}

struct question *question_get_by_num(int num)
{
	return NULL;
}

static void question_clear_all()
{
	if(quest_list!=NULL)
	{
		struct quest_node *temp2, *temp = quest_list;
		while(temp)
		{
			temp2 = temp;
			temp = temp->next;
			temp2->next = NULL;
		}
		quest_list = NULL;
	}
}
