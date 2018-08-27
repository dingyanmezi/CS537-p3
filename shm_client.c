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
///  hgjkhgjhvhjfyuhjgkjh
#define SHM_NAME "pujie_zhiqian"

// ADD NECESSARY HEADERS

// Mutex variables
typedef struct {
    		int pid;
    		char birth[25];
    		char clientString[10];
    		int elapsed_sec;
    		double elapsed_msec;
		double o; // meiyong
	} stats_t;
pthread_mutex_t* mutex;
//struct timeval *tt;
stats_t *candidate = NULL;
stats_t *shm_ptr = NULL;


//stats_t *clients[maxClients];

void exit_handler(int sig) {
    // ADD

    // critical section begins
	pthread_mutex_lock(mutex);
	
    // Client leaving; needs to reset its segment   
	candidate->pid = 0;
		

	pthread_mutex_unlock(mutex);
	// critical section ends

    exit(0);
}

int main(int argc, char *argv[]) {
	// ADD    
	if (argc != 2){
		exit(1);
	}
	struct sigaction act;
	act.sa_handler = exit_handler;
	const int PAGESIZE = getpagesize();

	int xianzai = 1;
	int maxClients = 63;	// 64 or 63 ?
	clock_t start = 0;
	clock_t end = 0; 
	
	

	int fd = shm_open(SHM_NAME, O_RDWR, 0660);
	if (fd == -1){
		exit(1);
	}

	shm_ptr = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shm_ptr == MAP_FAILED){
		exit(1);
	}
	mutex = (pthread_mutex_t*) shm_ptr;

	stats_t *clients[maxClients];
	for (int i = 0; i < maxClients; i++){
		clients[i] = shm_ptr + i + 1;
		
	}
	
	
    // critical section begins
    pthread_mutex_lock(mutex);
        
    // client updates available segment
	time_t mytime;
	mytime = time(NULL);

	start = clock();
	for (xianzai = 0; xianzai < maxClients; xianzai++){
		if (clients[xianzai]->pid == 0){
			candidate = clients[xianzai];
			clients[xianzai]->pid = getpid();

			strncpy(clients[xianzai]->birth, asctime(localtime(&mytime)), 24);
			strcat(clients[xianzai]->birth, "\0");
			strncpy(clients[xianzai]->clientString, argv[1], 9);
			strcat(clients[xianzai]->clientString, "\0");
			break;
		}
	}

	if (xianzai == maxClients){
		pthread_mutex_unlock(mutex);
		exit(1);
	}

	//char *c_time = ctime(&mytime);
	
    pthread_mutex_unlock(mutex);
    // critical section ends
        
	while (1) {
        
		// ADD
		if(sigaction(SIGINT, &act, NULL) == -1){
			exit(1);
		}
		if (sigaction(SIGTERM, &act, NULL) == -1){
			exit(1);
		}

		end = clock();
		candidate->elapsed_sec = difftime(time(NULL), mytime);
		candidate->elapsed_msec = (double)(end - start)/CLOCKS_PER_SEC * 1000;//(double)gettimeofday(tt, NULL);
						
		
        	sleep(1);
		
		printf("Active clients : ");
		fflush(stdout);
		for (int i = 0; i < maxClients; i++){
			if (clients[i]->pid){
				printf("%d ", clients[i]->pid);
				fflush(stdout);
			}	
		}
		printf("\n");
		fflush(stdout);
		
		// Print active clients
    	}

    return 0;
}


