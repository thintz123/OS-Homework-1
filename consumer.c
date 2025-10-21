#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#define BUFFER_SIZE 2

int main() {
    int shm_fd = shm_open("/my_shm", O_RDWR, 0666);
    if(shm_fd == -1) {
        perror("shm_open failed");
        exit(1);
    }

    void *shm_ptr = mmap(0, sizeof(int)* BUFFER_SIZE + 2 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    int *buffer = (int *)shm_ptr;
    int *in = (int *)(buffer + BUFFER_SIZE);
    int *out = (int *)(buffer + BUFFER_SIZE + 1);
    
    sem_t *empty = sem_open("/empty_sem", 0);
    sem_t *full = sem_open("/full_sem", 0);
    sem_t *mutex = sem_open("/mutex_sem", 0);

    for(int i = 0; i < 10; ++i) {
        sem_wait(full);
        sem_wait(mutex);

        int item = buffer[*out];
        printf("Consumed: %d from index %d\n", item, *out);
        *out = (*out + 1) % BUFFER_SIZE;

        sem_post(mutex);
        sem_post(empty);

        sleep(2);
    }

    munmap(buffer, sizeof(int)* BUFFER_SIZE + 2 * sizeof(int));
    close(shm_fd);

    sem_close(empty);
    sem_close(full);
    sem_close(mutex);

    return 0;


    
}