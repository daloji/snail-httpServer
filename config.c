#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAXBUF 1024 
#include "config.h"

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
   FILE *FILELOG = fopen("test", "w");
   if(FILELOG != NULL){
      //printf("[%s] %s ; %s \n ",tagToString(tag),buff,message);
      fprintf(FILELOG, "[%s] %s ; %s",tagToString(tag),buff,message);
       //fclose(FILELOG);
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



 

int read_int_config(char* config_line) {    
    char prm_name[MAXBUF];
    int val;
    sscanf(config_line, "%s %d\n", prm_name, &val);
    return val;
}

char* read_str_from_config_line(char* config_line) {  
	char *val = NULL;
	char prm_name[MAXBUF];
	if(config_line != NULL){
		val =(char*) malloc(sizeof(char)*strlen(config_line)+1);
		sscanf(config_line, "%s %s\n", prm_name, val);
	}  
	return val;
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
	FILE *fp = NULL;
    char buffer[MAXBUF];

	if(filename == NULL){
     fprintf(stderr, "fichier de configuration non trouvé\n");
     exit(EXIT_FAILURE);
	}
    
	if ((fp=fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Failed to open config file %s", filename);
        exit(EXIT_FAILURE);
    }
	t_config *config = (t_config*)malloc(sizeof(t_config));
	while(! feof(fp)) {
	  fgets(buffer, MAXBUF, fp);
	  if(buffer[0] == '#' || strlen(buffer) < 4) {
            continue;
      }
	  if(strstr(buffer, "DIRECTORY")){
		char *tmp =  read_str_from_config_line(buffer);
		config->wwwDirectory = (char*)malloc(sizeof(char)*strlen(tmp)+1);
		memcpy(config->wwwDirectory,tmp,strlen(tmp)+1);
		free(tmp);
      }
	  
	  if(strstr(buffer, "LOGFILE")){
		char *tmp =  read_str_from_config_line(buffer);
		config->filelog = (char*)malloc(sizeof(char)*strlen(tmp)+1);
		memcpy(config->filelog,tmp,strlen(tmp)+1);
		free(tmp);
      }
	  
	  if(strstr(buffer, "PORT")){
		config->port =  read_int_config(buffer);
		printf("%d",config->port);
      }
	  
	  if(strstr(buffer, "NBPROCESS")){
		config->maxProcess =  read_int_config(buffer);
		printf("%d",config->maxProcess);
      }
	  
	  if(strstr(buffer, "NBTHREAD")){
		config->maxThread =  read_int_config(buffer);
		printf("%d",config->maxThread);
      }
	  
	}
	fclose(fp);
	
	return config;
}