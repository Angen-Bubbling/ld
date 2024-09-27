#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ringbuffer.h"

typedef struct circular_buffer circular_buffer;

void cb_init(circular_buffer *cb, size_t capacity, size_t sz)
{
    cb->buffer = malloc(capacity * sz);
    if (cb->buffer == NULL)
    {
        // handle error, e.g., return or log
        return;
    }
    cb->buffer_end = (char *)cb->buffer + capacity * sz;
    cb->capacity = capacity;
    cb->count = 0;
    cb->sz = sz;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;

    // Initialize the mutex
    pthread_mutex_init(&cb->lock, NULL);
}

void cb_free(circular_buffer *cb)
{
    // Free the buffer
    free(cb->buffer);
    
    // Destroy the mutex
    pthread_mutex_destroy(&cb->lock);

    // Clear out other fields too, just to be safe
    cb->buffer = NULL;
    cb->buffer_end = NULL;
    cb->capacity = 0;
    cb->count = 0;
    cb->sz = 0;
    cb->head = NULL;
    cb->tail = NULL;
}

int cb_push_back(circular_buffer *cb, const void *item)
{
    // Lock the mutex to ensure exclusive access
    pthread_mutex_lock(&cb->lock);

    if (cb->count == cb->capacity)
    {
        // Buffer is full, handle error
        pthread_mutex_unlock(&cb->lock);  // Unlock before returning
        return -1;
    }

    // Copy the item into the head position
    memcpy(cb->head, item, cb->sz);

    // Move the head pointer forward
    cb->head = (char *)cb->head + cb->sz;
    if (cb->head == cb->buffer_end)
        cb->head = cb->buffer;  // Wrap around

    // Increment the count
    cb->count++;

    // Unlock the mutex
    pthread_mutex_unlock(&cb->lock);
    return 0;
}

int cb_pop_front(circular_buffer *cb, void *item)
{
    // Lock the mutex to ensure exclusive access
    pthread_mutex_lock(&cb->lock);

    if (cb->count == 0)
    {
        // Buffer is empty, handle error
        pthread_mutex_unlock(&cb->lock);  // Unlock before returning
        return -1;
    }

    // Copy the item from the tail position
    memcpy(item, cb->tail, cb->sz);

    // Move the tail pointer forward
    cb->tail = (char *)cb->tail + cb->sz;
    if (cb->tail == cb->buffer_end)
        cb->tail = cb->buffer;  // Wrap around

    // Decrement the count
    cb->count--;

    // Unlock the mutex
    pthread_mutex_unlock(&cb->lock);
    return 0;
}
