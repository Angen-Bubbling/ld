#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stddef.h>
#include <pthread.h>

// 环形缓冲区的结构体定义
typedef struct circular_buffer
{
    void *buffer;         // 数据缓冲区
    void *buffer_end;     // 数据缓冲区的结束位置
    size_t capacity;      // 缓冲区中最大元素数量
    size_t count;         // 缓冲区中当前元素数量
    size_t sz;            // 每个元素的大小
    void *head;           // 指向头部（写入位置）
    void *tail;           // 指向尾部（读取位置）
    pthread_mutex_t lock; // 用于线程安全的互斥锁
} circular_buffer;

/**
 * 初始化环形缓冲区
 * 
 * @param cb 指向环形缓冲区结构的指针
 * @param capacity 环形缓冲区可以容纳的最大元素数
 * @param sz 每个元素的大小
 */
void cb_init(circular_buffer *cb, size_t capacity, size_t sz);

/**
 * 释放环形缓冲区的资源
 * 
 * @param cb 指向环形缓冲区结构的指针
 */
void cb_free(circular_buffer *cb);

/**
 * 向环形缓冲区添加一个元素
 * 
 * @param cb 指向环形缓冲区结构的指针
 * @param item 要添加到缓冲区中的元素指针
 */
int cb_push_back(circular_buffer *cb, const void *item);


/**
 * 从环形缓冲区取出一个元素
 * 
 * @param cb 指向环形缓冲区结构的指针
 * @param item 用于存储取出元素的指针
 */
int cb_pop_front(circular_buffer *cb, void *item);

#endif // CIRCULAR_BUFFER_H
