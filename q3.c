#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

/// represents the thread data
struct arguments {
  int * scoreA;
  int * scoreB;
  int * counterTeamA;
  int * counterTeamB;
  pthread_mutex_t * scoreLock;
  sem_t **semArr;
  int argID;
};


// represents an individual thread
struct thread {
  pthread_t th;
  int tid;
};

/**
 * Gets a random thread to either pass or have the ball/puck intercepted
 * @param team Team 1 or team 0
 * @param min Smallest possible thread ID
 * @param max Largest possible thread ID
 * @return Thread id to mutate
 */
int randThread(int team, int min, int max);

/**
 * Frees thread_struct array memory
 * @param inArgArr Input array
 * @param size   Number of arguments
 */
void deallocate_arguments(struct arguments **inArgArr, int size);

/**
 * Allocates thread_struct array memory
 * @param inArgArr Input array
 * @param size   Number of arguments
 * @param scoreA Team A score
 * @param scoreB Team B score
 * @param counterTeamA Team A number of passes
 * @param counterTeamB Team B number of passes
 * @param scoreLock Mutex for scoring points
 * @param semArr Semaphore to lock the player
 * 
 */
void allocate_arguments(struct arguments **inArgArr, int size, 
  int *scoreA, int *scoreB, 
  int *counterTeamA, int *counterTeamB,
  pthread_mutex_t *scoreLock,
  sem_t **semArr); 

/**
 * @param inArgArr Argument array
 * @param size Arg array size
 */
void run_threads(struct arguments **inArgArr, struct thread **inThArr, int size); 
/**
 * Performs the operations for the game
 * @param arg Structure to pass into thread
 */
void *do_work(void *arg);

/**
 * Allocates semaphores for do_work
 * @param semArr Array of sems
 * @param size Size of semArr
 */
void allocate_sems(sem_t **semArr, int size); 

/**
 * deallocates semaphores for do_work
 * @param semArr Array of sems
 * @param size Size of semArr
 */
void deallocate_sems(sem_t **semArr, int size);

/**
 * Allocates threads structs
 * @param inThArr Thread array
 * @param size Size of thread array
 */
void allocate_threads(struct thread **inThArr, int size);

/**
 * Deallocates threads structs
 * @param inThArr Thread array
 * @param size Size of thread array
 */
void deallocate_threads(struct thread **inThArr, int size);


/**
 * Allocates threads structs
 * @param inThArr Thread array
 * @param size Size of thread array
 */
void allocate_threads(struct thread **inThArr, int size);

/**
 * Deallocates threads structs
 * @param inThArr Thread array
 * @param size Size of thread array
 */
void deallocate_threads(struct thread **inThArr, int size);

/**
 * @param semArr Array of semaphores
 * @param currentID Id of running sem
 * @param size Size of semArr
 */
void call_other_sems(sem_t **semArr, int currentID, int size);

int main(int argc, char *argv) {
  //Seed rng
  //Do not change seed
  srand(42);
  int scoreA = 0;
  int scoreB = 0;
  int counterTeamA = 0;
  int counterTeamB = 0;
  pthread_mutex_t scoreLock;
  int totalThreads = 10;
  struct thread *thArr[totalThreads];
  sem_t *semArr[totalThreads];
  allocate_threads(thArr, totalThreads);
  allocate_sems(semArr, totalThreads);
  struct arguments *arguments[totalThreads];
  allocate_arguments(arguments, totalThreads, &scoreA, &scoreB, &counterTeamA, &counterTeamB, &scoreLock, semArr);
  run_threads(arguments, thArr, totalThreads);
  deallocate_arguments(arguments, totalThreads);
  deallocate_sems(semArr, totalThreads);
  deallocate_threads(thArr, totalThreads);
  printf("\n\n");
  exit(0); 
}


void allocate_sems(sem_t **semArr, int size) {
  int iter;
  for (iter = 0; iter < size; iter++) {
    semArr[iter] = (sem_t *) calloc(1, sizeof(sem_t));
  }
}
void deallocate_sems(sem_t **semArr, int size) {
  int iter;
  for (iter = 0; iter < size; iter++) {
    free(semArr[iter]);
  }
}

void *do_work( void *args ) {
   /**
   * START THE GAME
   *  get a random number
   *  initialize sem of the very first random thread
   * THREADS
   * wait for a sem call
   *  while score is less than 10
   *    Pass if in team range
   *      Counter of team is incremented
   *      if counter == 5
   *        score of team incremented
   *      if score == 10
   *        game is won
   *        call all semaphores to stop the arguments
   *       
   *    Interc if not
   *      Counter of team reset
   *
   */
  struct arguments *inArgs = (struct arguments *) args;
  int toThread;
  while (*inArgs->scoreA < 10 && *inArgs->scoreB < 10) { 
    sem_wait(inArgs->semArr[inArgs->argID]);
    // score can get updated while the thread is waiting, so it will never get called if that's the case.
    // when a thread finishes the game, it gets called again, so it needs to check if the score is final to terminate
    if (*inArgs->scoreA < 10 && *inArgs->scoreB < 10) {
      if (inArgs->argID < 5) {
        toThread = randThread(0, 0, 10);
      } else {
        toThread = randThread(1, 0, 10);
      }
      // team A
      if (inArgs->argID < 5) {
        // pass
        if (toThread < 5) {
          pthread_mutex_lock(inArgs->scoreLock);
          *inArgs->counterTeamA += 1;
          printf("%s%d%s%d%s%d\n", "[Pass: Team: 0] Thread: ", inArgs->argID, ", Pass to Thread: ", toThread, ", Counter Team 0: ", *inArgs->counterTeamA);
          pthread_mutex_unlock(inArgs->scoreLock);
          // score if the counter is 5
          if (*inArgs->counterTeamA == 5) {
            pthread_mutex_lock(inArgs->scoreLock);
            *inArgs->scoreA += 1;
            *inArgs->counterTeamA = 0;
            printf("%s%d%s%d%s%d\n", "[Team: 0 scored a net!] Score Team 0: ", *inArgs->scoreA, ", Score Team 1: ", *inArgs->scoreB, ", Counter Team 0: ", *inArgs->counterTeamA);
            printf("%s%d\n", "[Team 0] Counter Reset: ", *inArgs->counterTeamA);
            pthread_mutex_unlock(inArgs->scoreLock);
            if (*inArgs->scoreA == 10) {   
              call_other_sems(inArgs->semArr, inArgs->argID, 10);
            }
          }


        // interception
        } else {
          *inArgs->counterTeamA = 0;
          printf("%s%d%s%d%s%d\n", "Interception: Team: 0] Thread: ", inArgs->argID, ", intercepted by Thread: ", toThread, ", Counter Team 0: ", *inArgs->counterTeamA);
        }
      // team b
      } else {
        // interception
        if (toThread < 5) {
          *inArgs->counterTeamB = 0;
          printf("%s%d%s%d%s%d\n", "Interception: Team: 1] Thread: ", inArgs->argID, ", intercepted by Thread: ", toThread, ", Counter Team 1: ", *inArgs->counterTeamB);

        // pass
        } else {
          pthread_mutex_lock(inArgs->scoreLock);
          *inArgs->counterTeamB += 1;
          printf("%s%d%s%d%s%d\n", "[Pass: Team: 1] Thread: ", inArgs->argID, ", Pass to Thread: ", toThread, ", Counter Team 1: ", *inArgs->counterTeamB);
          pthread_mutex_unlock(inArgs->scoreLock);
          // score if the counter is 5
          if (*inArgs->counterTeamB == 5) {
            pthread_mutex_lock(inArgs->scoreLock);
            *inArgs->scoreB += 1;
            *inArgs->counterTeamB = 0;
            printf("%s%d%s%d%s%d\n", "[Team: 1 scored a net!] Score Team 0: ", *inArgs->scoreA, ", Score Team 1: ", *inArgs->scoreB, ", Counter Team 1: ", *inArgs->counterTeamB);
            printf("%s%d\n", "[Team 1] Counter Reset: ", *inArgs->counterTeamB);
            pthread_mutex_unlock(inArgs->scoreLock);
            if (*inArgs->scoreB == 10) {
              call_other_sems(inArgs->semArr, inArgs->argID, 10);
            }
          }

        }

      }
      
      sem_post(inArgs->semArr[toThread]);
    } 
  }
  return NULL;

}

//Do not modify
//Team must be 0 or 1
//0 for team 0
//1 for team 1
//min- minimum thread id to generate
//max- maximum thread id to generate
//Min= 0
//Max= 10
//Generated in [0, 10)
int randThread(int team, int min, int max) {
  double randNum = (double)rand() / (double)RAND_MAX;
  //Generate random semaphore to send the ball/puck/etc. to
  //On the team
  if (randNum < 0.6) {
    double randNum = ((double)rand() / (double)RAND_MAX)*(max*1.0/2*1.0);
    //send to teammate
    if (team == 0) {
      return (int) randNum;  
    }
    
    if (team == 1) {
      return (int)randNum+5;
    }
  } else {
    double randNum = 5.0*((double)rand() / (double)RAND_MAX);
    //send to other team
    if (team == 1) {
      return (int) randNum;  
    }
    
    if (team == 0) {
      return (int)randNum+5;
    }
  }
}

void deallocate_arguments(struct arguments **inArgArr, int size) {
  int iter;

  for (iter = 0; iter < size; iter++) {
    free(inArgArr[iter]);
  }
}

void run_threads(struct arguments **inArgArr, struct thread **inThArr, int size) {
  int iter;
  sem_post(inArgArr[0]->semArr[0]);
  for (iter = 0; iter < size; iter++) {
    pthread_create(&inThArr[iter]->th, NULL, do_work, (void *) inArgArr[iter]);
  } for (iter = 0; iter < size; iter++) {
    pthread_join(inThArr[iter]->th, NULL);
  }
}

void call_other_sems(sem_t **semArr, int currentID, int size) {
  int iter;
  for (iter = 0; iter < size; iter++) {
    if (iter != currentID) {
      sem_post(semArr[iter]);
    }
  }
}

void allocate_arguments(struct arguments **inArgArr, int size, 
  int *scoreA, int *scoreB, 
  int *counterTeamA, int *counterTeamB,
  pthread_mutex_t *scoreLock,
  sem_t **semArr) {
  pthread_mutex_init(scoreLock, NULL);
  int iter;
  for (iter = 0; iter < size; iter++) {
    inArgArr[iter] = (struct arguments *) calloc(1, sizeof(struct arguments));
    inArgArr[iter]->scoreA = scoreA;
    inArgArr[iter]->scoreB = scoreB;
    inArgArr[iter]->counterTeamA = counterTeamA;
    inArgArr[iter]->counterTeamB = counterTeamB;
    inArgArr[iter]->scoreLock = scoreLock;
    inArgArr[iter]->argID = iter;
    inArgArr[iter]->semArr = semArr;
  }
}

void allocate_threads(struct thread **inThArr, int size) {
  int iter;
  for (iter = 0; iter < size; iter++) {
    inThArr[iter] = (struct thread *) calloc(1, sizeof(struct thread));
    inThArr[iter]->tid = iter;
  }
}

void deallocate_threads(struct thread **inThArr, int size) {
  int iter;
  for (iter = 0; iter < size; iter++) {
    free(inThArr[iter]);
  }
}