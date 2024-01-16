/**
 * @file dining.c
 * @author Leiah Nay (nayl@mcmaster.ca)
 * @brief An implementation of the dining philosophers problem
 * @version 0.1
 * @date 2023-11-14
 * 
 * @copyright Copyright (c) 2023
 * 
 * Command to Run: 
 * gcc -g -Wall dining.c -lpthread -o dining
 * 
 */



#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUMBER_OF_THREADS 5
#define NUMBER_OF_FORKS 5


void *phil(void * n);  /*  */
void pickup_forks(int i);
void return_forks(int i);

pthread_mutex_t mutexs[NUMBER_OF_FORKS];
pthread_cond_t forks[NUMBER_OF_FORKS];
bool forksAvailable[NUMBER_OF_FORKS];


int main ( int argc , const char * argv [] )
{
    //initialzie mutexes and condition variables
    for(int j = 0; j < NUMBER_OF_FORKS; j++) {
        pthread_mutex_init(&mutexs[j], NULL);
        pthread_cond_init(&forks[j], NULL);
        forksAvailable[j] = true;
    }
    
    //create philosopher threads
    int i ;
    pthread_t philosophers [ NUMBER_OF_THREADS ] ;

    int *arg = malloc(sizeof(*arg));
    int *arg1 = malloc(sizeof(*arg1));
    int *arg2 = malloc(sizeof(*arg2));
    int *arg3 = malloc(sizeof(*arg3));
    int *arg4 = malloc(sizeof(*arg4));

    *arg = 0;
    *arg1 = 1;
    *arg2 = 2;
    *arg3 = 3;
    *arg4 = 4;
    
    
    /* establish the threads */
    pthread_create(&philosophers[0] , 0 , phil , arg) ;
    pthread_create(&philosophers[1] , 0 , phil , arg1) ;
    pthread_create(&philosophers[2] , 0 , phil , arg2 ) ;
    pthread_create(&philosophers[3] , 0 , phil , arg3 ) ;
    pthread_create(&philosophers[4] , 0 , phil , arg4 ) ;
    
    /* now wait for the threads to finish */
    for ( i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_join( philosophers[i] , NULL ) ;
    
}


/**
 * the lifecycle for a philosopher
 */
void *phil(void *n) {
    int i = *((int *) n);

    while(true) {
        pickup_forks(i);

        //eat
        sleep(rand() % 3 + 1);

        return_forks(i);

        //think
        sleep(rand() % 3 + 1);
    }

    pthread_exit(0);
}   

/**
 * philosopher picks up both forks 
 * 
 * i is the philosopher's number
 */
void pickup_forks(int i) {
    printf("Philosopher %d picks up forks. \n", i);
    bool done = false;

    int left = i;
    int right = (i + 1) % NUMBER_OF_FORKS;

    while(!done) {
        // try to get both locks 
        pthread_mutex_lock(&mutexs[left]);
        pthread_mutex_lock(&mutexs[right]);

        //if the forks are available, pick them up
        if (forksAvailable[left]) {
            if (forksAvailable[right]) {

                forksAvailable[left] = false;
                forksAvailable[right] = false;

                done = true;
            }
        }
        else { //otherwise wait
            pthread_cond_wait(&forks[left], &mutexs[left]);
            pthread_cond_wait(&forks[right], &mutexs[right]);
        }

        //unlock both locks if pick up was unsuccessful
        pthread_mutex_unlock(&mutexs[left]);
        pthread_mutex_unlock(&mutexs[right]);

    }
    
}


/**
 * philosopher returns both forks 
 * 
 * i is the philosopher's number
 */
void return_forks(int i) {
    printf("Philosopher %d returns forks. \n", i);

    int left = i;
    int right = (i + 1) % NUMBER_OF_FORKS;

    forksAvailable[left] = true;
    forksAvailable[right] = true;

    //unlock both mutexes
    pthread_mutex_unlock(&mutexs[left]);
    pthread_mutex_unlock(&mutexs[right]);

    //signal condition variables
    pthread_cond_broadcast(&forks[left]);
    pthread_cond_broadcast(&forks[right]);
}