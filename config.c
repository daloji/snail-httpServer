#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAXBUF 1024 
#include "config.h"

//FILE *FILELOG;

void logger(typelog tag,const char* message){
   char buff[100];
   const char* level =NULL;
   time_t now;
   time(&now); 
   strftime (buff, 100, "%d/%m/%Y %H:%M:%S", localtime (&now));
   FILE *FILELOG = fopen("test", "w");
   if(FILELOG != NULL){
      printf("[%s] %s ; %s \n ",tagToString(tag),buff,message);
      fprintf(FILELOG, "[%s] %s ; %s",tagToString(tag),buff,message);
      //fprintf(FILELOG,"[%s] %s ; %s \n ",tagToString(tag),buff,message);
     // fwrite("rrr" , 1 , 3 , FILELOG );
      //fclose(FILELOG);
   }
}

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
 * \fn t_config * readconfig(char *filename)
 * \brief  lecture du fichier de configuration donnée en entrée
 * \details   la fonction est utilisée pour la lecture du fichier de configuration
 *	     du serveur, cette fonction est la premiere a etre executé par le serveur et
 * 	     renvoie la structure t_config (cf #t_config) qui gére la configuration du serveur
 *   
 * \param  char *filename  fichier de configuration
 * \return  renvoi la structure  t_config (cf #t_config) en cas d'echec erreur de demarrage du serveur
 */
t_config * readconfig(char *filename){
  
  if(filename == NULL){
     fprintf(stderr, "fichier de configuration non trouvé\n");
     exit(EXIT_FAILURE);
  }
  
  t_config *config = (t_config*)malloc(sizeof(t_config));
  if(config == NULL){
     fprintf(stderr, "Echec de la creation variable config -> allocation memoire impossible\n");
     exit(EXIT_FAILURE);
  }
  FILE *file = fopen (filename, "r");
  if (file != NULL){ 
     char line[MAXBUF];
     char *tmp;
     while(fgets(line, sizeof(line), file) != NULL){
	char *key;
	char *value;
	tmp = (char*) malloc(sizeof(char)*strlen(line)+1);
	memcpy(tmp,line,strlen(line)+1);
        key = strtok((char *)line,DELIM);
	if(strcmp(DIRECTORY,key)==0){
	   value = strstr((char *)tmp,DELIM);
	   if(value != NULL){
	      value = strtok(value, "\n");
	      value = value + strlen(DELIM);
	      config->wwwDirectory = (char*)malloc(sizeof(char)*strlen(value)+1);
	      memcpy(config->wwwDirectory,value,strlen(value)+1);
	   }
	}
	
	if(strcmp(LOGFILE,key)==0){
	   value = strstr((char *)tmp,DELIM);
	   if(value != NULL){
	      value = strtok(value, "\n");
	      value = value + strlen(DELIM);
	      config->filelog = (char*)malloc(sizeof(char)*strlen(value)+1);
	      memcpy(config->filelog,value,strlen(value)+1);
	   }
	}
	
	if(strcmp(PORT,key)==0){
	  value = strstr((char *)tmp,DELIM);
	  if(value != NULL){
	      value = strtok(value, "\n");
	      value = strstr((char *)tmp,DELIM);
	      value = value + strlen(DELIM);
	      config->port = atoi(value);
	  }else{
	      config->port =-1;
	  }
	}
	
	if(strcmp(NBPROCESS,key)==0){
	  value = strstr((char *)tmp,DELIM);
	  if(value != NULL){
	      value = strtok(value, "\n");
	      value = strstr((char *)tmp,DELIM);
	      value = value + strlen(DELIM);
	      config->maxProcess = atoi(value);
	  }
	}
	
	if(strcmp(NBTHREAD,key)==0){
	  value = strstr((char *)tmp,DELIM);
	  if(value != NULL){
	     value = strtok(value, "\n");
	      value = strstr((char *)tmp,DELIM);
	      value = value + strlen(DELIM);
	      config->maxThread = atoi(value);
	  }
	}
	
	 free(tmp);
     } // End if file
  
    fclose(file);
  }else{
    logger(FATAL,"erreur lecture fichier");
    config = NULL;
  }             
                
  return config;

}