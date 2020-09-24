#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


void *do_work(void *arg);

struct threads_and_buffer { 
   pthread_mutex_t lock;
   pthread_mutex_t lock_even;
   pthread_mutex_t lock_odd;
   pthread_cond_t *is_empty;
   pthread_cond_t *is_full;
   int *buffer;
   int *buffer_size;
   int *num_updates_even;
   int *num_updates_odd;
   int t;
}; 

int main(int argc, char *argv) 
{

  int t = 0;

  pthread_mutex_t lock, lock_even, lock_odd;
  pthread_cond_t is_empty;
  pthread_cond_t is_full;
  int buffer=0;
  int buffer_size=0;
  int num_updates_even=0;
  int num_updates_odd=0;

  int num_threads=10;
  pthread_t worker_thread[num_threads];
  struct threads_and_buffer *worker_struct[ num_threads ];

  pthread_mutex_init(&lock, NULL);
  pthread_mutex_init(&lock_even, NULL);
  pthread_mutex_init(&lock_odd, NULL);
  pthread_cond_init(&is_empty, NULL);
  pthread_cond_init(&is_full, NULL);
  

  // Creating child threads
  for (t = 0; t < num_threads; t++) 
  {

    worker_struct[t] = (struct threads_and_buffer *) calloc(1, sizeof(struct threads_and_buffer));

    worker_struct[t]->lock = lock;
    worker_struct[t]->lock_even = lock_even;
    worker_struct[t]->lock_odd = lock_odd;
    worker_struct[t]->is_empty = &is_empty;
    worker_struct[t]->is_full = &is_full;
    worker_struct[t]->buffer = &buffer;
    worker_struct[t]->buffer_size = &buffer_size;
    worker_struct[t]->num_updates_even = &num_updates_even;
    worker_struct[t]->num_updates_odd = &num_updates_odd;
    worker_struct[t]->t = t;

    if (pthread_create(&worker_thread[t], NULL, do_work, (void *) worker_struct[t])) 
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
  }

  printf("\nThe nunber of odd updates to the buffer was: %d", num_updates_odd);
  printf("\nThe number of even updates to the buffer was: %d", num_updates_even);

  printf("\n\n");

  return 0;
}

void *do_work(void *arg) 
{
  struct threads_and_buffer* work_thread = (struct threads_and_buffer *)arg;
    
  int myTid= work_thread->t;
  pthread_mutex_t lock = work_thread->lock;
  pthread_mutex_t lock_even = work_thread->lock_even;
  pthread_mutex_t lock_odd = work_thread->lock_odd;
  pthread_cond_t *is_empty = work_thread->is_empty;
  pthread_cond_t *is_full = work_thread->is_full;
  int *buffer = work_thread->buffer;
  int *buffer_size = work_thread->buffer_size;
  int *num_updates_even = work_thread->num_updates_even;
  int *num_updates_odd = work_thread->num_updates_odd;
  


  while( *num_updates_even < 10 && *num_updates_odd < 10 )
  {

    //Code for even threads
    //Add to buffer
    if (myTid%2==0)
    { 
      
      pthread_mutex_lock(&lock);
      while (*buffer_size == 1)
      {
        fprintf(stderr,"\n[Tid %d] Waiting\n", myTid);
        pthread_cond_wait(is_empty, &lock);
      }
      
      pthread_mutex_lock(&lock_even);
      *buffer=myTid;
      printf("\n[Tid %d] Buffer is: %d\n", myTid, *buffer);
      (*buffer_size)++;
      (*num_updates_even)++;
      pthread_mutex_unlock(&lock_even);
         
      
      pthread_cond_signal(is_full);
      pthread_mutex_unlock(&lock);
      usleep(100000);
    }
  
    //Code for odd threads
    //Remove from buffer
    else if (myTid%2==1)
    {
      
      pthread_mutex_lock(&lock);
      while (*buffer_size == 0)
      {
        fprintf(stderr,"\n[Tid %d] Waiting\n", myTid);
        pthread_cond_wait(is_full, &lock);
      }
      
      pthread_mutex_lock(&lock_odd);
      int myBuffer = *buffer;
      fprintf(stderr,"\n[Tid %d] Buffer is: %d\n", myTid, myBuffer);
      (*buffer_size)--;
      (*num_updates_odd)++;
      pthread_mutex_unlock(&lock_odd);
         
      
      pthread_cond_signal(is_empty);
      pthread_mutex_unlock(&lock);
      
      usleep(100000);
    }
  }
  

  return NULL;
}
