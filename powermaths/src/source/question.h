#ifndef __QUESTION_H__
#define __QUESTION_H__

struct question
{
	int number;
	char *caption;
	char *group;
	char *level;
	char *type;
	char *topic;
	char *stopic;
	char *filename;
};

struct quest_node
{
	struct question *quest;
	void *next;
};

extern struct quest_node *quest_list;

extern int question_load_questions(char *sql);
extern int question_get_total();
extern struct question *question_get_by_num(int num);

#endif
