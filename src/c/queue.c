
#include "queue.h"

/*!
@brief create and return a new queue item.
*/
queue_item * queue_new_item(
    void * data
){
    queue_item * tr = calloc(1, sizeof(queue_item));
    tr     -> data = data;
    return tr;
}


/*!
@brief Create a new empty queue.
@returns a new empty queue.
*/
queue * queue_new()
{
    queue * tr = calloc(1, sizeof(queue));
    
    tr -> head      = NULL;
    tr -> tail      = NULL;
    tr -> length    = 0;

    return tr;
}


/*!
@brief Free the memory allocated for a queue.
@param in tofree - The queue to free.
@details Does not free the data in the queue. Only the queue metadata.
*/
void queue_free(
    queue * tofree
){
    void * d = queue_dequeue(tofree);
    
    while(d != NULL) {
        d = queue_dequeue(tofree);
    }

    free(tofree);
}


/*!
@brief Free the memory allocated for a queue.
@param in q - The queue to append to.
@param in toadd - The item to add to the queue.
*/
void queue_enqueue(
    queue * q,
    void  * toadd
){
    queue_item * i = queue_new_item(toadd);

    if(q -> head == NULL) {
        q -> head = i;
        q -> tail = i;
        q -> length = 1;
    } else {
        q -> tail -> next = i;
        q -> tail         = i;
        q -> length      += 1;
    }
}


/*!
@brief Return the item at the front of a queue or NULL.
@param in q - The queue to remove things from.
*/
void * queue_dequeue(
    queue * q
){
    if(q -> head == NULL) {
        return NULL;

    } else {

        queue_item * item_tr = q -> head;

        q -> head = q -> head -> next;

        void * tr = item_tr -> data;
        free(item_tr);

        q -> length --;

        return tr;

    }
}
