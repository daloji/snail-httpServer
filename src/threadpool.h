#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#ifndef _THPOOL_
#define _THPOOL_

typedef struct taskThread t_taskThread;

typedef struct processFunction t_processFunction;

typedef struct threadPool t_threadPool;

typedef struct argumentThread t_argumentThread;

/**
 *  \brief Structure representant l'argument du pointeur de fonction 
 *  \details argument envoyé dans chaque  pointeur de fonction d'un thread
 */
struct argumentThread{
  int poolId;  /*!< identifiant du Thread)*/
  
  int socketFd; /*!< socket utilisé par le Thread)*/
  
  char *threadname; /*!< nom du  Thread)*/
  
  char *wwwDirectory; /*!< une espace de travaille (ex: /var/www/) contenant les sources de votres site web utilisé par le Thread)*/
};

/**
 *  \brief Structure representant  une liste chainée representant la liste des Threads a lancée 
 *  \details cette liste chainée est utilisée directement par le PoolThread
 */
struct taskThread{
  char * nameThead; /*!< nom du  Thread)*/
  
  pthread_t thread; /*!< instance du  Thread)*/
  
  t_processFunction *executeFunction; /*!< ponteur de fonction (fonction a executé par le Thread)*/
  
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

extern t_threadPool *THREADPOOL;

t_threadPool*  createThreadPool(int thread_count, void (*function)(void *),void *arg);

void releaseThreadPool(t_threadPool*  threadpool);

int threadpool_destroy(t_threadPool  *pool);

char * getFileName(char* directory,char* msg);

#endif