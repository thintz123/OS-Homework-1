#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BUFFER_SIZE 2


int main() {
    int shm_fd = shm_open("/my_shm", O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1) {
        perror("shm_open failed");
        exit(1);
    }
    ftruncate(shm_fd, sizeof(int)*BUFFER_SIZE + 2 * sizeof(int));
    if(ftruncate == -1) {
        perror("ftruncate failed");
        exit(1);
    }
    void *shm_ptr = mmap(0, sizeof(int)*BUFFER_SIZE + 2 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    int *buffer = (int *)shm_ptr;   
    int *in = (int *)(buffer + BUFFER_SIZE);
    int *out = (int *)(buffer + BUFFER_SIZE + 1);

    *in = 0;
    *out = 0;

    sem_t *empty = sem_open("/empty_sem", O_CREAT, 0666, BUFFER_SIZE);
    sem_t *full = sem_open("/full_sem", O_CREAT, 0666, 0);
    sem_t *mutex = sem_open("/mutex_sem", O_CREAT, 0666, 1);

     if (empty == SEM_FAILED || full == SEM_FAILED || mutex == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }


    for (int i = 0; i < 10; i++) {
        sem_wait(empty);
        sem_wait(mutex);

       buffer[*in] = i;
        printf("Produced: %d at index %d\n", i, *in);
        *in = (*in + 1) % BUFFER_SIZE;

        sem_post(mutex);
        sem_post(full);

        sleep(1);
    }

    munmap(shm_ptr, sizeof(int)* BUFFER_SIZE + 2 * sizeof(int));
    close(shm_fd);
    
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);

    sem_unlink("/empty_sem");
    sem_unlink("/full_sem");
    sem_unlink("/mutex_sem");
    shm_unlink("/my_shm");

    return 0;

}
