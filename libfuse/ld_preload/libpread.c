#define _OPEN_THREADS
#define _GNU_SOURCE

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

typedef ssize_t (* pread_type)(int, void*, size_t, off_t);

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

ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    pread_type real = (pread_type) dlsym(RTLD_NEXT, "pread");
	// if (atomic_load(&thread_count) == 0) {
	// 	pthread_create(&io_worker, NULL, &thread_work, NULL);
	// }
	if (g_buffer == buf) {
		printf("this is data pread,fd:%d,count:%lu,offset:%ld,buffer is:%p,g_buffer is:%p\n",fd,count,offset,buf,g_buffer);
		g_buffer += count;
	} else {
        printf("this is index pread,fd:%d,count:%lu,offset:%ld,buffer is:%p,g_buffer is:%p\n",fd,count,offset,buf,g_buffer);
	}
    return real(fd, buf, count, offset);
}