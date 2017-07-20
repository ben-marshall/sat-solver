
#include <stdlib.h>

#ifndef QUEUE_H
#define QUEUE_H



/*!
@brief A single item in the queue.
*/
typedef struct queue_item_t queue_item;
struct queue_item_t {
    void       * data; //<! Pointer to the data item of the queue.
    queue_item * next; //<! Pointer to the next item in the queue.
};


/*!
@brief A simple queue for any old data type.
*/
typedef struct queue_t {
    queue_item * head;      //<! First item in the queue.
    queue_item * tail;      //<! Last item in the queue.
    unsigned int length;    //<! Number of items in the queue.
} queue;


/*!
@brief Create a new empty queue.
@returns a new empty queue.
*/
queue * queue_new();


/*!
@brief Free the memory allocated for a queue.
@param in tofree - The queue to free.
*/
void queue_free(
    queue * tofree
);


/*!
@brief Free the memory allocated for a queue.
@param in q - The queue to append to.
@param in toadd - The item to add to the queue.
*/
void queue_enqueue(
    queue * q,
    void  * toadd
);


/*!
@brief Return the item at the front of a queue or NULL.
@param in q - The queue to remove things from.
*/
void * queue_dequeue(
    queue * q
);

#endif
