#ifndef __QUESTION_H__
#define __QUESTION_H__

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

extern struct QuestionNode *qlist;

extern int question_load_questions(char *table_name);
extern int question_get_total();
extern struct Question *question_get_by_num(int num);

#endif
