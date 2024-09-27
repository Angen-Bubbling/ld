#define _OPEN_THREADS
#define _GNU_SOURCE
#define QUEUE_DEPTH 10

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <liburing.h>
#include <stdatomic.h>
#include <dlfcn.h>

#include <pthread.h>
#include "libpread.h"
#include "ringbuffer.h"

typedef ssize_t (*pread_type)(int, void *, size_t, off_t);

pthread_t io_thread;
atomic_int thread_count = ATOMIC_VAR_INIT(0);
void *global_buffer;
struct circular_buffer ring_buffer;

struct read_info {
	int fd;
	void *buf;
	size_t count;
	off_t offset;
};

void *io_worker(void *parm)
{
	struct read_info info;
	struct io_uring ring;
	struct io_uring_sqe *sqe;
	struct io_uring_cqe **cqes = (struct io_uring_cqe **)malloc(
		QUEUE_DEPTH * sizeof(struct io_uring_cqe *));
	int io_count = 0;
	int loop_count = 0;
	io_uring_queue_init(QUEUE_DEPTH, &ring, 0);
	while (true) {
		if (io_count < QUEUE_DEPTH) {
			if (cb_pop_front(&ring_buffer, &info)) {
				sqe = io_uring_get_sqe(&ring);
				io_uring_prep_read(sqe, info.fd, info.buf,
						   info.count, info.offset);
				sqe->user_data = info.buf;
				io_count++;
			}
			if (loop_count > 10 && io_count > 0) {
				loop_count = 0;
				io_uring_submit(&ring);
			}
		}
		if (io_count > 0) {
			unsigned int nfinish;
			nfinish = io_uring_peek_batch_cqe(&ring, cqes,
							  QUEUE_DEPTH);
			if (nfinish > 0) {
				for (int i = 0; i < nfinish; i++) {
					void *io_buf = cqes[i]->user_data;
					//唤醒+错误处理
					io_uring_cqe_seen(&ring, cqes[i]);
				}
				io_count--;
			}
		}
		loop_count++;
	}
}

int set_buf(void *buffer)
{
	global_buffer = buffer;
	return 0;
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
	pread_type real = (pread_type)dlsym(RTLD_NEXT, "pread");

	if (global_buffer == buf) {
		printf("this is data pread,fd:%d,count:%lu,offset:%ld,buffer is:%p,g_buffer is:%p\n",
		       fd, count, offset, buf, global_buffer);
		if (atomic_load(&thread_count) == 0) {
			cb_init(&ring_buffer, 1000000,
				sizeof(struct read_info));
			struct read_info info = { .fd = fd,
						  .buf = buf,
						  .count = count,
						  .offset = offset };
			cb_push_back(&ring_buffer, &info);
			pthread_create(&io_thread, NULL, &io_worker, NULL);
		}
		
		global_buffer += count;
	} else {
		printf("this is index pread,fd:%d,count:%lu,offset:%ld,buffer is:%p,g_buffer is:%p\n",
		       fd, count, offset, buf, global_buffer);
		return real(fd, buf, count, offset);
	}
}