#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "logger.h"
#define BUFFER 1024
char* pipe_named ="/tmp/fifo";


static int msgid;

/**
 * \fn void  *loggMessageTofile(void *argument)
 * \brief    logger pour les messages remontés par le server  
 * \details  le logger est utilisé par un thread separé dans le but est de logger les messages venant du serveur
 * \param  void *argument  contenat le fichier de log 
 */
void  *loggMessageTofile(void *argument){
  FILE *fp = NULL;
  char *filelog = (char *)argument;
  if(filelog == NULL){
    fprintf(stderr, "erreur fichier de log non valorise\n");
    return;
  }
  
  msgid = msgget(IPC_PRIVATE, MSGPERM|IPC_CREAT|IPC_EXCL);
  int size,rc;
  while(1){
    t_logmessage logmessage;
    rc = msgrcv(msgid,&logmessage,sizeof(t_logmessage),1,0); 
    if(rc< 0){
      printf("msgrcv failed, rc=%d\n", rc);
      exit(EXIT_FAILURE);
    }
    
    fp = fopen (filelog, "a+");
    if(fp==NULL){
      fprintf(stderr, "erreur lors de l'ouverture du fichier de log \n");
      return;
    }
    //creation  element pour ajouter dans la liste de log
   fprintf(fp, "%s\n",logmessage.message);
   fclose(fp);
 }

}

/**
 * \fn const char* tagToString(typelog tag)
 * \brief    renvoi la chaine de caractere correspondant a l'enum typelog (cf #typelog))  
 * \details  logger utilisé pour le messages d'information venant du server
 * \param  typelog tag (cf #typelog) niveau de sévérité 
 * \param  const char* message  message d'information
 * \return char *  niveau de log 
 */
const char* tagToString(typelog tag){
  char *level = NULL;
  switch (tag){
    case INFO:level = "INFO";
    break;
    case DEBUG:level ="DEBUG";
    break;
    case SEVERE:level = "SEVERE";
    break;
    case FATAL:level = "FATAL";
    break;
    default: level = " ";
  }
  return level;
}



/**
 * \fn logger(typelog tag,const char* message)
 * \brief    logger (INFO,SEVERE,FATAL,DEBUG,WARNING)  
 * \details  logger utilisé pour le messages d'information venant du server
 * \param  typelog tag (cf #typelog) niveau de sévérité du message thread_count nombre de thread a crée dans le pool 
 * \param  const char* message  message d'information
 */
void logger(typelog tag,const char* message){
   char buff[100];
   const char* level =NULL;
   time_t now;
   struct tm *infotime;
   time(&now);
   infotime = localtime(&now);
   strftime (buff, 100, "%d/%m/%Y %H:%M:%S",infotime);
 
   if(message!=NULL){
      level = tagToString(tag);
      int initlogger = strlen(message) +strlen(level)+strlen(buff)+10;
      char *log =(char *) malloc(initlogger* sizeof(char));
      if(log == NULL){
	  fprintf(stderr, "erreur lors de l'allocation memoire\n");
	  exit(EXIT_FAILURE);
      }
      sprintf(log, "%s; [%s] %s",buff,level,message);
      addLog(log);
      free(log);	
    }
}

void addLog(char *message){
   t_logmessage logmessage; 
   logmessage.type=1;
   logmessage.pidEmetteur = getpid();
   memcpy(logmessage.message,message,strlen(message)+1);
   int rc = msgsnd(msgid,&logmessage,sizeof(t_logmessage),0);
   if(rc<0){
    fprintf(stderr, "erreur lors de l'envoi de la file de message : %s \n",strerror(errno));
   }
}




void initlogger(){
  key_t cle;
  msgid = msgget(cle,IPC_CREAT|IPC_EXCL|0600);
  if(msgid<0){
     fprintf(stderr, "erreur lors creation \n");
  }
   printf("initlogger msg: %d\n",msgid);
}


void appendQueueLog(t_logItem* list,char* message){
  if(message == NULL){
    return;
  }
  t_logItem* logitem = (t_logItem*)malloc(sizeof(t_logItem));
  if(logitem == NULL){
     fprintf(stderr, "erreur lors de l'allocation memoire\n");
     exit(EXIT_FAILURE);
  }
  logitem->message = (char*)malloc((strlen(message)+1)*sizeof(char));
  if(logitem->message == NULL){
    fprintf(stderr, "erreur lors de l'allocation memoire\n");
    exit(EXIT_FAILURE);
  }
  memcpy(logitem->message,message,strlen(message)+1);
  logitem->next = NULL;
  if(list == NULL){
     list = logitem;
      // printf("=======%s==========\n",list->message);
  }else{
     t_logItem * pt = list;
      while(pt!=NULL){
	pt =pt->next;
      }
      pt=logitem;
    }
}

void printQueue(t_logItem* list){
 t_logItem* pt;
  if(list!=NULL){
    pt=list;
   while(pt!=NULL){
    // printf(">>>>%s<<<<<<<\n",pt->message);
     pt=pt->next;
   }
 }
}
char* dequeingLog(){
  char *message = NULL;
  if(QUEUELOG != NULL){
    if(QUEUELOG->front != NULL){
	if(QUEUELOG->front->message != NULL){
	  int size =strlen(QUEUELOG->front->message);
	  message = (char*)malloc(size+1*sizeof(char));
	  if(message == NULL){
	    fprintf(stderr, "erreur lors de l'allocation memoire\n");
	    exit(EXIT_FAILURE);	
	  }
	  memcpy(message,QUEUELOG->front->message,size+1);
	  free(QUEUELOG->front->message);
	  QUEUELOG->front=QUEUELOG->front->next;
	}
    }
  }
  return message;
}
