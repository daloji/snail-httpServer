#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include "logger.h"
#define BUFFER 1024
char* pipe_named ="/tmp/fifo";

void  *loggMessageTofile(void *argument){
	int fd ;
	FILE *filelog = NULL;
    char buf[BUFFER];
	char *logfile = argument;
	fd = open(pipe_named, O_RDONLY | O_NONBLOCK);
	if(fd<0){
		fprintf(stderr,"Error ouverture tube nommée! %s\n", strerror(errno));
		exit(1);
	}
	if(logfile != NULL){
		while(1){
			if(read(fd, buf, BUFFER)<0){
				filelog = fopen(logfile, "a+");
				if(filelog != NULL){
					printf("%s",buf);
					fwrite(buf,1,strlen(buf),filelog);
					fclose(filelog);
				}
			}
		 sleep(10);
		}
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
	   sprintf(log, "[%s] %s ; %s \n",level,buff,message);
	   char *message = (char*) malloc(BUFFER*sizeof(char));
	   if(message == NULL){
		  fprintf(stderr, "Erreur lors de l'allocation memoire \n");
		  exit(EXIT_FAILURE);
	   }
	   strcat(message,log);
	   int fd= open(pipe_named, O_WRONLY| O_NONBLOCK);
	   if(fd>0){
		  write(fd, message, strlen(message));  
	   }
	   if(message != NULL){
		   free(message);
	   }
	   if(log != NULL){
		   free(log);
	   }
	}
}




void initlogger(){

    mkfifo(pipe_named, 0666);   


	//fdq = open(LOGGER_TUBE, O_RDONLY);
	/*if(QUEUELOG== NULL){
		QUEUELOG = (t_queingLog*)malloc(sizeof(t_queingLog));
		if(QUEUELOG == NULL){
		  fprintf(stderr, "erreur lors de l'allocation memoire\n");
	      exit(EXIT_FAILURE);
		}
		QUEUELOG->front = NULL;
		QUEUELOG->rear = NULL;
		QUEUELOG->size = 0;
		
	}*/
	
}


t_logItem* createQueueLog(const char* message){
	if(message == NULL){
		return NULL;
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
	return logitem;
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



void queingLog(t_logItem *log){
	if(log == NULL){
	  return;	
	}
	if(QUEUELOG != NULL){
		if(QUEUELOG->front == NULL){
			QUEUELOG->front = log;
			QUEUELOG->rear = log;
		}else{
			QUEUELOG->rear->next = log;
			QUEUELOG->rear = log;
			QUEUELOG->size++;
		}
	}
}