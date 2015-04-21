#ifndef NODE_H
#define NODE_H

//Create a struct to hold nodes for that are to be stored with data and put in queue

struct nodeT
{
	int ref;
	void *data;
	struct nodeT *next;
};				

typedef struct nodeT node;

node *create_node(void *, node *);

void destroy_node(node *);

#endif
