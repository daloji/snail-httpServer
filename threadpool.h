#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#ifndef _THPOOL_
#define _THPOOL_

typedef struct taskThread t_taskThread;

typedef struct processFunction t_processFunction;

typedef struct threadPool t_threadPool;

typedef struct argumentThread t_argumentThread;


struct argumentThread{
  int poolId;
  int socketFd;
  char *threadname;
  char *wwwDirectory;
};


struct taskThread{
  char * nameThead;
  pthread_t thread;
  t_processFunction *executeFunction;
  int poolId;
  int threadId;
  int state;
  t_taskThread *next;
};

struct processFunction{
    void (*function)(void *);
    void *argument;
};

struct threadPool {
  t_taskThread *listthread;
  int index;
  int poolsize;
};


t_threadPool*  createThreadPool(int thread_count, void (*function)(void *),void *arg);

int threadpool_destroy(t_threadPool  *pool);

char * getFileName(char* directory,char* msg);

#endif