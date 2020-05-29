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

#define DOMADVISE 0

int main(int argc, char *argv[])
{
    int res;
    int fd;
    int len;
    int *addr;


    unsigned long size = NUM_PAGES *  1024 * sizeof(int);
    fd = shm_open(STORAGE_ID, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("open");
        return 10;
    }

    res = ftruncate(fd, size);
    if (res == -1)
    {
        perror("ftruncate");
        return 20;
    }

    addr = (int*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd , 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        return 30;
    }

    uint64_t i=0;
    while(i < size/sizeof(int)) {
        *(addr+i) = i;
        i++;
    }
    printf("Data populated\n");
    sleep(10);

    printf("Access started\n");
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
    //sleep(15);
    printf("Sum : %lld" , sum);

    sleep(100); 
    munmap(addr , size);
    fd = shm_unlink(STORAGE_ID);
    if (fd == -1)
    {
        perror("unlink");
        return 100;
    }

    return 0;
}
