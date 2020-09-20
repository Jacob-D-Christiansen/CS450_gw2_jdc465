#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>


struct arguments{
sem_t * sem;
int * scoreA;
int * scoreB;
int * counterTeamA;
int * counterTeamB;
pthread_mutex_t * scoreLock;
int tid;
};



//Prototypes
int randThread(int team, int min, int max);
void *do_work(void *arg);



int main(int argc, char *argv) {

  //Seed rng
  //Do not change seed
  srand(42);

  
  int totalThreads=10;
  struct arguments * args=(struct arguments *)malloc(sizeof(struct arguments)*(totalThreads));
  
  //write code here

  
  free(args);
  printf("\n\n");
  exit(0); 
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
int randThread(int team, int min, int max)
{
    double randNum = (double)rand() / (double)RAND_MAX;
    //Generate random semaphore to send the ball/puck/etc. to
    //On the team
    if (randNum<0.6)
    {
      double randNum = ((double)rand() / (double)RAND_MAX)*(max*1.0/2*1.0);
      //send to teammate
      if (team==0)
      {
      return (int) randNum;  
      }
      if (team==1)
      {
        return (int)randNum+5;
      }
    }
    //Interception by other team
    else
    {
      double randNum = 5.0*((double)rand() / (double)RAND_MAX);
      //send to other team
      if (team==1)
      {
      return (int) randNum;  
      }
      if (team==0)
      {
      return (int)randNum+5;
      }
    }
}
