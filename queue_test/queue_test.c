#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define __NR_fifo_init 448
#define __NR_fifo_free 449
#define __NR_fifo_in_spinlock 450
#define __NR_fifo_out_spinlock 451

long fifo_init(unsigned int size, unsigned int locktype)
{
	return syscall(__NR_fifo_init, size, locktype);
}

long fifo_free(void *lfifo)
{
	return syscall(__NR_fifo_free, lfifo);
}

long fifo_in(void *lfifo, void *buf,
			 unsigned int len)
{
	return syscall(__NR_fifo_in_spinlock, lfifo, buf, len);
}

long fifo_out(void *lfifo, void *buf,
			  unsigned int len)
{
	return syscall(__NR_fifo_out_spinlock, lfifo, buf, len);
}
void *producer_work(void *fifo)
{
	pid_t tid = gettid();
	fifo_in(fifo, &tid, sizeof(pid_t));
}
void *comsumer_work(void *fifo)
{
	int success = 0;
	while (true)
	{
		pid_t tid;
		if (fifo_out(fifo, &tid, sizeof(pid_t)) != 0)
		{
			printf("tid is %d\n", tid);
			success++;
		}
		if (success == 100)
		{
			printf("success to 10");
			break;
		}
	}
}

int main()
{
	int output;
	pthread_t producer;
	pthread_t comsumer;
	long ret;
	void *fifo;

	ret = fifo_init(100 * sizeof(pid_t), 1);
	if (ret != -1)
		fifo = (void *)ret;
	else
		exit(-1);
	for (int i = 0; i < 100; i++)
	{
		pthread_create(&producer, NULL, producer_work, fifo);
	}
	pthread_create(&comsumer, NULL, comsumer_work, fifo);

	if (pthread_join(comsumer, NULL) != 0)
	{
		fprintf(stderr, "Error joining comsumer\n");
		return 1;
	}

	return 0;
}