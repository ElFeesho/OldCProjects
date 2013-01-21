#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple linked list

typedef struct _scan_list
{
	unsigned int pos;
	void *next;
} scan_list;

static void scan_list_add(scan_list **list, unsigned int pos);
static void scan_list_remove(scan_list **list, scan_list *node);
static int scan_list_count(scan_list **list);
static void scan_list_print(scan_list **list);

void scan_file(const char *filename, unsigned int value)
{
	scan_list *value_list = NULL;
	FILE *input_file = fopen(filename,"r");

	if(input_file == NULL)
	{
		return;
	}

	if(value_list == NULL)
	{
		unsigned int buffer = 0;
		unsigned int pos = 0;
		while(!feof(input_file))
		{
			int am = 0;
			am = fread(&buffer,1,4,input_file);
			fseek(input_file,ftell(input_file)-3,SEEK_SET);

			if(buffer==value)
			{
				scan_list_add(&value_list, pos);
			}

			pos++;
			if(am!=sizeof(value))
				break;
		}
	}
	fclose(input_file);

	printf("Do you wish to print the findings? (There are %d) [Y/n]: ", scan_list_count(&value_list));

	char choice = 'n';
	if(scanf("%c\n", &choice)!=0)
	{
		if(choice == 'y' || choice == 'Y')
		{
			scan_list_print(&value_list);
		}
	}
}

void scan_list_add(scan_list **list, unsigned int pos)
{
	if(*list == NULL)
	{
		*list = malloc(sizeof(scan_list));
		(*list)->pos = pos;
		(*list)->next = NULL;
	}
	else
	{
		scan_list *new_node = malloc(sizeof(scan_list));
		new_node->pos = pos;
		new_node->next = *list;
		*list = new_node;
	}
}

void scan_list_remove(scan_list **list, scan_list *node)
{
	if(*list == node) // it's the first node
	{
		*list = node->next;
		free(node);
	}else{
		scan_list *temp = *list;
		while(temp->next!=node)
			temp = temp->next;
		temp->next = node->next;
		free(node);
	}
}

int scan_list_count(scan_list **list)
{
	if(*list == NULL)
		return 0;
	scan_list *iter = *list;
	int count = 0;
	while(iter!=NULL)
	{
		count++;
		iter=iter->next;
	}
	return count;
}

void scan_list_print(scan_list **list)
{
	scan_list *temp = *list;
	while(temp!=NULL)
	{
		printf("Location: %X\n\n",temp->pos);
		temp=temp->next;
	}
}
