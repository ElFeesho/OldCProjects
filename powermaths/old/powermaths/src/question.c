#include <stdio.h>
#include <stdlib.h>
#include "database.h"

struct Question
{
	int number;
	char *caption;
	char *filename;
};

struct QuestionNode
{
	struct Question *qstn;
	void *next;
};

struct QuestionNode *qlist = NULL;

static void question_clear_all();

int question_load_questions(char *table_name)
{
	question_clear_all();
	int count = 0;
	char *sql = malloc(192);
	memset(sql,'\0',192);
	sprintf(sql,"SELECT * FROM %s ORDER BY number DESC",table_name);
	sqlite3_stmt *query = database_query(sql);
	while(sqlite3_step(query)==SQLITE_ROW)
	{
		count++;
		if(qlist == NULL)
		{
			qlist = malloc(sizeof(struct QuestionNode));
			qlist->qstn = malloc(sizeof(struct Question));
			qlist->qstn->caption = malloc(255);
			memset(qlist->qstn->caption,'\0',255);
			qlist->qstn->filename = malloc(128);
			memset(qlist->qstn->filename,'\0',128);
			qlist->qstn->number = sqlite3_column_int(query,1);
			strcpy(qlist->qstn->caption, (char*)sqlite3_column_text(query,2));
			strcpy(qlist->qstn->filename, (char*)sqlite3_column_text(query,3));
			qlist->next = NULL;
		}
		else
		{
			struct QuestionNode *temp = malloc(sizeof(struct QuestionNode));
			temp->qstn = malloc(sizeof(struct Question));
			temp->qstn->caption = malloc(255);
			memset(temp->qstn->caption,'\0',255);
			temp->qstn->filename = malloc(128);
			memset(temp->qstn->filename,'\0',128);
			temp->qstn->number = sqlite3_column_int(query,1);
			strcpy(temp->qstn->caption, (char*)sqlite3_column_text(query,2));
			strcpy(temp->qstn->filename, (char*)sqlite3_column_text(query,3));
			temp->next = qlist;
			qlist = temp;
		}
	}
	return count;
}

int question_get_total()
{
	struct QuestionNode *temp = qlist;
	int count = 0;
	while(temp)
	{
		count++;
		temp = temp->next;
	}
	return count;
}

struct Question *question_get_by_num(int num)
{

	return NULL;
}

static void question_clear_all()
{
	if(qlist!=NULL)
	{
		struct QuestionNode *temp2, *temp = qlist;
		while(temp)
		{
			temp2 = temp;
			temp = temp->next;
			temp2->next = NULL;
		}
		qlist = NULL;
	}
}
