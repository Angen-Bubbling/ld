#define _OPEN_THREADS

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <liburing.h>
#include <stdatomic.h>

#include <pthread.h>
#include "libpread.h"

pthread_t io_worker;
atomic_int thread_count = ATOMIC_VAR_INIT(0);
void *g_buffer;

int thread_work()
{
	return 0;
}

int set_buf(void *buffer)
{
	g_buffer = buffer;
	return 0;
}

ssize_t pread(int filedes, void *buf, size_t nbytes, off_t offset)
{
	if (atomic_load(&thread_count) == 0) {
		pthread_create(&io_worker, NULL, &thread_work, NULL);
	}
	if (g_buffer == buf) {
		printf("this is data pread\n");
	} else {
        printf("this is index pread");
	}
    return 
}