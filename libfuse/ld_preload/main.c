#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
    int fd,ret;
    char buffer[100];
    fd = open("/dev/nvme0n1p1",O_RDONLY);
    ret = pread(fd,buffer,1,10);
    printf("ret:%d",ret);
    return 0;
}