#define _SVID_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "random_r.h"
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <math.h>
#include <pthread.h>

#define NBR_PHILO 5
#define EATING_MEAN 9
#define EATING_STD 3
#define THINKING_MEAN 11
#define THINKING_STD 7
#define MAX_EATING_TIME 100

pthread_mutex_t chopstiks[5];
int philoTimeEating[NBR_PHILO];
int philoTimeThinking[NBR_PHILO];

void makeSemaphore(int size);
void* philosopher(void* philo);
void thinking(const int philo, const int thinkingTime, const int totalThinkingTime);
void eating(const int philo, const int eatingTime, const int totalEatingTime);
void initializeMutexes(int size);

int main(void){
  int i, *iPtr;
  pthread_t philo[NBR_PHILO];
  
  initializeMutexes(NBR_PHILO);
  // Create the threads (philosophers).
  for(i = 0; i < NBR_PHILO; i++){
	iPtr = (int*) malloc(sizeof(int));
	*iPtr = i;
	pthread_create(&philo[i], NULL, philosopher, (void*)iPtr);
  }
  
  // Waits for each thread to terminate
  for(i = 0; i < NBR_PHILO; i++)
    pthread_join(philo[i], NULL);
    
  printf("**************************************************************************\n");
  for(i = 0; i < NBR_PHILO; i++){
  	printf("Philosopher %i: Total eating time [%d s], Total thinking time [%d s]\n",i 
  	,philoTimeEating[i], philoTimeThinking[i]);
  }
    return 0;
}

void eating(const int philo, const int eatingTime, const int totalEatingTime){
    printf("Philo %d is eating for %d seconds. [Time spent eating = %d]\n", philo
    , eatingTime, totalEatingTime);
    sleep(eatingTime);
}

void thinking(const int philo, const int thinkingTime, const int totalThinkingTime){
    printf("Philo %d is thinking for %d seconds.[Time spent thinking =%d]\n", philo, thinkingTime, totalThinkingTime);
    sleep(thinkingTime);
}

// This function simulates the philosophers eating and thinking.
void* philosopher(void* arg){  

  int philo =   *((int*) arg);
  int eatingTime = 0;
  int totalEatingTime = 0;
  int thinkingTime = 0;
  int totalThinkingTime =0;
  free(arg);
  unsigned int state = philo;

  while(totalEatingTime <= MAX_EATING_TIME){
    // try to acquire chopsticks if available.
    while (1) {
    	pthread_mutex_lock(&chopstiks[philo]);
    	int rtrn = pthread_mutex_trylock(&chopstiks[(philo+1) % 5]);
    
    	if (rtrn == EBUSY)
    		pthread_mutex_unlock(&chopstiks[philo]);
    	else break;
    	sleep(1);
    }
    	
   	/******* philosopher i is eating for a random amount of time.*******/
    if((eatingTime = randomGaussian_r(EATING_MEAN, EATING_STD, &state)) < 0)
    	eatingTime = 0;

	eating(philo, eatingTime, totalEatingTime);
    totalEatingTime += eatingTime;
   	 
    // done eating, Release the chopsticks.
    pthread_mutex_unlock(&chopstiks[philo]);
	pthread_mutex_unlock(&chopstiks[(philo+1) % 5]);

   	 /****** Philo is thinking for a random amount of time. ********/
   	if((thinkingTime = randomGaussian_r(THINKING_MEAN, THINKING_STD, &state)) < 0) 
    	thinkingTime = 0;
	totalThinkingTime += thinkingTime;
 	thinking(philo, thinkingTime, totalThinkingTime);  
  } 
  printf("Philo %i has finished eating. [Total time spent eating = %i]\n", philo, totalEatingTime);
  philoTimeEating[philo] = totalEatingTime;
  philoTimeThinking[philo] = totalThinkingTime;
  return NULL;
}

// Function to initialize semaphore's value to 1.
void initializeMutexes(int size){
	for (int i = 0; i < size; i++)
		pthread_mutex_init(&chopstiks[i], NULL);
}

