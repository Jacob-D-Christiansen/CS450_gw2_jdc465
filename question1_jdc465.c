#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// created struct to hold the old global variables
struct arguments
{
   pthread_mutex_t lock;
   int *count;
   int tid;
};


void *do_work(void *arg);



int main(int argc, char *argv) 
{
   // added intalization inside of main
   pthread_mutex_t lock;
   pthread_mutex_init(&lock, NULL);
   
   int count = 0;
   int num_threads=100;
   
   struct arguments *arg_thread[num_threads];
   pthread_t worker_thread[num_threads];

     // Creating child threads
     int t;
     for (t=0; t < num_threads; t++) 
     {
       // added arguments struct creation
       arg_thread[t] = (struct arguments *)
            calloc(1, sizeof(struct arguments));
       arg_thread[t]->lock = lock;
       arg_thread[t]->count = &count;
       arg_thread[t]->tid = t;
       
       if (pthread_create(&worker_thread[t], NULL, 
                         do_work, (void *) arg_thread[t])) 
       {
         fprintf(stderr,"Error while creating thread #%d\n",t);
         exit(1);
       }

       fprintf(stdout,"Waiting for child thread to complete\n");
     }
     
     // Joining with child threads
     for (t=0; t < num_threads; t++) 
     {
       if (pthread_join(worker_thread[t], NULL)) 
       {
         fprintf(stderr,"Error while joining with child thread #%d\n",t);
         exit(1);
       }
       free(arg_thread[t])
     }

     printf("\nCount: %d (should be: %d)", 
         count, ((num_threads-1)*(num_threads))/2);
     printf("\n\n");
     exit(0); 
}


void *do_work(void *arg) 
{
  // unpack arguments
  struct arguments *args = (struct arguments *) arg;
  int *count = args->count;
  int tid = args->tid;
  pthread_mutex_t lock = args->lock;
  
  pthread_mutex_lock(&lock);
  // changed to pointer
  (*count) += tid;
  pthread_mutex_unlock(&lock);
  

  return NULL;
}

