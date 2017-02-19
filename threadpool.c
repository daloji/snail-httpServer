#include <stdlib.h>
#include <string.h>
#include "threadpool.h"

#include <pthread.h>


/**
 * \brief    creation pool de thread   t_threadPool  
 * \details   le pool de thread contient le nombre de thread a cree  (liste chainée)
 * (ex. GET index.html ) la fonction create la structure t_threadPool (cf #t_threadPool)
 *      t_threadPool->poolId  (numero du processus)
 * 	t_httpRequest->socketFd (socket)
 * 	t_httpRequest->threadname (nom du thread)
 * 	t_httpRequest->executeFunction (pointeur de fonction)
 * 
 * \param  int thread_count nombre de thread a crée dans le pool 
 * \param  void function fonction a executé par le thread 
 * \param  void argument (cf #t_argumentThread)argument de la fonction a executée 
 * \return  t_threadPool (cf #t_threadPool)  structure contenant le pool de thread
 */
t_threadPool* createThreadPool(int thread_count,void (*function)(void *),void *argument){
  int i;
  t_threadPool* pt =NULL;
  t_taskThread *pts =NULL;
  t_threadPool *pool = (t_threadPool*) malloc(sizeof(t_threadPool));
  t_argumentThread* ag = (t_argumentThread *)argument;
 
  if(pool == NULL){
    fprintf(stderr, "Echec de la creation PoolThread  -> allocation memoire impossible\n");
    return NULL;
  }
  
  pool->index = 0;
 
  for(i=0;i<thread_count;i++){
    
    t_argumentThread *arg = (t_argumentThread *) malloc(sizeof(t_argumentThread));
    if(arg == NULL){
	fprintf(stderr, "Echec de la creation argumentThread  -> allocation memoire impossible\n");
	return NULL;
    }
  
    arg->poolId = getpid();
    arg->socketFd = ag->socketFd;
    arg->threadname = (char*) malloc(sizeof(char)*20);
    
    sprintf(arg->threadname, "%d Thread %d ",getpid(),i);
    t_taskThread* taskthread = (t_taskThread *) malloc(sizeof(t_taskThread));
    if(taskthread == NULL){
	fprintf(stderr, "Echec de la creation taskThread  -> allocation memoire impossible\n");
	return NULL;
    }
    taskthread->executeFunction = (t_processFunction *) malloc(sizeof(t_processFunction));
    if(taskthread->executeFunction == NULL){
	fprintf(stderr, "Echec de la creation taskThread function -> allocation memoire impossible\n");
	return NULL;
    }
    
    arg->wwwDirectory = (char*) malloc(sizeof(char)*(strlen(ag->wwwDirectory)+1));
    if(arg->wwwDirectory !=NULL ){
      memcpy(arg->wwwDirectory,ag->wwwDirectory,strlen(ag->wwwDirectory)+1);   
    }
       
    taskthread->executeFunction->function = function;
    taskthread->executeFunction->argument = arg;
    
    taskthread->nameThead = (char *) malloc((strlen(arg->threadname)+1)*sizeof(char));
    if(taskthread->nameThead == NULL){
	fprintf(stderr, "Echec de la creation taskThread  -> allocation memoire impossible\n");
	return NULL;
    }
  
    memcpy(taskthread->nameThead,arg->threadname,strlen(arg->threadname)+1);  
    
    if(pthread_create(&taskthread->thread, NULL,(void *)taskthread->executeFunction->function,taskthread->executeFunction->argument ) != 0){
       printf(" Echec de lors de l'appel a pthread_create   \n");
       exit(EXIT_FAILURE);
    }
    
    taskthread->threadId = i;
    taskthread->next = pts;
    pts = taskthread;
     
  }
 
  pool->listthread = pts;
  pool->poolsize = thread_count;

  return pool;
 
}
