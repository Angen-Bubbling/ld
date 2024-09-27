#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "libpread.h"

const size_t kPageSize = 1024ul * 4;
const size_t kGigabyte = 1024ul * 1024 * 1024;

// int main()
// {
// 	int aio_pthd = 10;
//     int block_len = 1;
//     int file_len = 63;
//     int fd = open("/mnt/nvme0/iozone/tt",O_RDONLY);
//     block_len *= kPageSize, file_len *= kGigabyte;
// 	size_t block_idx[aio_pthd];
// 	void *buffer_uring[aio_pthd];
// 	void *buffer_pread[aio_pthd];
// 	for (int i = 0; i < aio_pthd; i++) {
// 		block_idx[i] = hash(i) + hash(getpid());
// 		buffer_uring[i] = aligned_alloc(kPageSize, block_len);
// 		size_t offset = block_idx[i] % file_len / kPageSize * kPageSize;

// 		sqe->user_data = i;


// 	}
// }