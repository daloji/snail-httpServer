#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"



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


void initlogger(){
	if(quelog== NULL){
		quelog = (t_queingLog*)malloc(sizeof(t_queingLog));
		if(quelog == NULL){
		  fprintf(stderr, "erreur lors de l'allocation memoire\n");
	      exit(EXIT_FAILURE);
		}
		quelog->front = NULL;
		quelog->rear = NULL;
		quelog->queue = NULL;
		quelog->size = 0;
		
	}
	
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
   time(&now); 
   strftime (buff, 100, "%d/%m/%Y %H:%M:%S", localtime (&now));
   if(message!=NULL){
	   
	  // level = tagToString(tag);
	   //printf("%s" ,level);
	   /*
	   int initlogger = strlen(message) +strlen(level)+strlen(buff)+10;
	   char *log =(char *) malloc(initlogger* sizeof(char));
	   if(log == NULL){
		  fprintf(stderr, "erreur lors de l'allocation memoire\n");
	      exit(EXIT_FAILURE);
		}
	   sprintf(log, "[%s] %s ; %s",level,buff,message);
	   printf("%s \n",log);
	   */
   }
}