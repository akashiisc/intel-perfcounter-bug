#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>


#define STORAGE_ID "/SHM_TEST"
#define NUM_PAGES 2000000

int main(int argc, char *argv[])
{
    int res;
    int fd;
    pid_t pid;
    int *addr;

    unsigned long size = NUM_PAGES *  1024 * sizeof(int);
    fd = shm_open(STORAGE_ID, O_RDONLY, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("open");
        return 10;
    }

    addr = (int*)mmap(NULL, size, PROT_READ , MAP_SHARED , fd , 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        return 30;
    }


    printf("Access started\n");
    int i;
    time_t start, end;
    int total_times = 1;
    int j = 0;
    int x;
    unsigned long sum = 0;
    time(&start);
    while(j < total_times) {
        i = 0;
        unsigned long iterations = (size/sizeof(int));
        while(i < iterations) {
            unsigned long random_num = rand() % ((size/2) / sizeof(int));
            x = *(addr+random_num);
            sum+=x;
            i++;
        }
        j++;
    }
    time(&end);
    printf("Time taken : %d\n",end-start);
    return 0;
}
