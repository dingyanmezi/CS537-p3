#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
 #include <sys/time.h>

#define SHM_NAME "pujie_zhiqian"
#define PAGESIZE getpagesize()

// ADD NECESSARY HEADERS
typedef struct {
	   	 int pid;
		 char birth[25];
		 char clientString[10];
		 int elapsed_sec;
		 double elapsed_msec;
		double o; // meiyong

	} stats_t;
// Mutex variables
pthread_mutex_t* mutex;
pthread_mutexattr_t mutexAttribute;
stats_t *shm_ptr = NULL;
//const int PAGESIZE = getpagesize();

void exit_handler(int sig) 
{
    // ADD
    	munmap(shm_ptr, PAGESIZE);
	if (shm_unlink(SHM_NAME) == -1 ){
		exit(1);
	}
	exit(0);
}

int main(int argc, char *argv[]) 
{
    // ADD
    	struct sigaction act;
	act.sa_handler = exit_handler;
	int j;
	int counter = 0;
	int maxClients = 63;
	//stats_t *babe;
	
	// Creating a new shared memory segment
	int fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);
	if (fd == -1){
		printf("fd gg.\n");
		exit(1);
	}
	if (ftruncate(fd, PAGESIZE) == -1){
		printf("ftruncate gg.\n");
		exit(1);
	}

	shm_ptr = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shm_ptr == MAP_FAILED){
		printf("shm_ptr gg.\n");
		exit(1);
	}

    // Initializing mutex
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);

	mutex = (pthread_mutex_t*)shm_ptr;

	pthread_mutex_init(mutex, &mutexAttribute);
	
	stats_t *clients[maxClients];
	for (int i = 0; i < maxClients; i++){
		clients[i] = shm_ptr + i + 1;
		clients[i]->pid = 0;
	}

    while (1) 
	{
		sleep(1);

		if(sigaction(SIGINT, &act, NULL) == -1){
			exit(1);
		}
		if (sigaction(SIGTERM, &act, NULL) == -1){
			exit(1);
		}

		for (j = 0; j < maxClients; j++){
			if (clients[j]->pid){
				//babe = clients[j];
				printf("%d, pid : %d, birth : %s, elapsed : %d s %f ms, %s\n", counter, clients[j]->pid, clients[j]->birth, clients[j]->elapsed_sec, 
				clients[j]->elapsed_msec, clients[j]->clientString);
				
			}
		}
		counter++;
		
		//if (j == maxClients){
		//	pthread_mutex_unlock(mutex);
		//	printf("mei dong xi.\n");
		//	exit(1);
		//}
		// ADD

        
    }

    return 0;
}
