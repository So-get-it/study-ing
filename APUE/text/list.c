#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct node_s
{
	int           data;
	struct node_s *next;
}node_t;

int main(int argc, char *argv[])
{
	node_t *head = NULL;
	node_t *new_node;
	node_t *tail;
	int    i;

	for(i=0; i<10; i++)
	{
		new_node = malloc(sizeof(node_t));
		memset(new_node, 0, sizeof(node_t));
		new_node->data = i;
		new_node->next = NULL;

		if(head == NULL)
		{
			head = new_node;
		}
		else
		{
			tail->next = new_node;
		}
		tail = new_node;		
	}
	
	node_t *node = NULL;
	for(node = head; node!=NULL; node = node->next)
	{
		printf("%d\n",node->data);
	}

	return 0;
}



