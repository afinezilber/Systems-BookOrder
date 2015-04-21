#include "node.h"
#include <stdlib.h>


//Create a node to store its data and the return ptr to new node. 
//Keep count of how many times the node is referenced.
node *create_node(void *data, node *next) {
    node *nodeThread = (node *) malloc(sizeof(node));
    if (nodeThread) {
		nodeThread->ref = 0;
		nodeThread->next = next;
        nodeThread->data = data;
        return nodeThread;
    }
    else
        return NULL;
}

//Free all memory associated with node
void destroy_node(node *nodeT) {
    free(nodeT);
}
