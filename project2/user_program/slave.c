#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define BUF_SIZE 512
#define PAGE_SIZE 4096


int main (int argc, char* argv[])
{
    char buf[BUF_SIZE];
    int i, dev_fd, file_fd;// the fd for the device and the fd for the input file
    size_t ret, file_size = 0, data_size = -1;
    char file_name[50];
    char method[20];
    char ip[20];
    struct timeval start;
    struct timeval end;
    double trans_time; //calulate the time between the device is opened and it is closed
    char *kernel_address, *file_address;


    strcpy(file_name, argv[1]);
    strcpy(method, argv[2]);
    strcpy(ip, argv[3]);

    if( (dev_fd = open("/dev/slave_device", O_RDWR)) < 0)//should be O_RDWR for PROT_WRITE when mmap()
	{
            perror("failed to open /dev/slave_device\n");
            return 1;
	}
    gettimeofday(&start ,NULL);
    if( (file_fd = open (file_name, O_RDWR | O_CREAT | O_TRUNC)) < 0)
	{
            perror("failed to open input file\n");
            return 1;
	}

    if(ioctl(dev_fd, 0x12345677, ip) == -1) // connect to master in the device
	{
            perror("ioclt create slave socket error\n");
            return 1;
	}


    size_t offset = 0;
    switch(method[0]){
        case 'f'://fcntl : read()/write()
            do
                {
                    ret = read(dev_fd, buf, sizeof(buf)); // read from the the device
                    write(file_fd, buf, ret); //write to the input file
                    file_size += ret;
                }while(ret > 0);
            break;
        case 'm':
            while (1) {
                ret = ioctl(dev_fd, 0x12345678);
                if (ret == 0) {
                    file_size = offset;
                    break;
                }

                posix_fallocate(file_fd, offset, ret);
                file_address = mmap(NULL, ret, PROT_WRITE, MAP_SHARED, file_fd, offset);
                kernel_address = mmap(NULL, ret, PROT_READ, MAP_SHARED, dev_fd, offset);

                memcpy(file_address, kernel_address, ret);
                offset += ret;
                int cnt = 0;
                while(PAGE_SIZE * cnt < ret) {
                    ioctl(dev_fd, 0, file_address + PAGE_SIZE * cnt);
                    cnt = cnt + 1;
                }
                munmap(file_address, ret);
                munmap(kernel_address, ret);
            }
            break;
	}
    if(ioctl(dev_fd, 0x1235679) == -1)// end receiving data, close the connection
	{
            perror("ioclt client exits error\n");
            return 1;
	}
    gettimeofday(&end, NULL);
    trans_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)*0.0001;
    printf("Slave Transmission time: %lf ms, File size: %d bytes\n", trans_time, (int)file_size / 8);


    close(file_fd);
    close(dev_fd);
    return 0;
}


