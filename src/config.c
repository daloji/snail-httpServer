#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAXBUF 1024 
#include "config.h"


/**
 * \fn int read_int_config(char* config_line)
 * \brief  transformation d'une donnee de type chainde de caractere en entier utilisée dans la configuration
 * \details   la fonction est utilisée pour la lecture du fichier de configuration
 *	     du serveur, cette fonction est la premiere a etre executé par le serveur 
 *   
 * \param  char *string configuration
 * \return entier 
 */
int read_int_config(char* config_line) {    
  char prm_name[MAXBUF];
  int val;
  sscanf(config_line, "%s %d\n", prm_name, &val);
  return val;
}

/**
 * \fn char* read_str_from_config_line(char* config_line)
 * \brief  transformation d'une donnee de type chainde de caractere en entier
 * 	  utilisée dans las configuration
 * \details   la fonction est utilisée pour la lecture du fichier de configuration
 *	     du serveur, cette fonction est la premiere a etre executé par le serveur 
 *   
 * \param  char *string configuration
 * \return entier 
 */
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
;	  
    if(strstr(buffer, "CERTIFICAT")){
      char *tmp =  read_str_from_config_line(buffer);
      config->certFile = (char*)malloc(sizeof(char)*strlen(tmp)+1);
      memcpy(config->certFile,tmp,strlen(tmp)+1);
   
      free(tmp);
    }
    
    if(strstr(buffer, "PRIVATE_KEY")){
      char *tmp =  read_str_from_config_line(buffer);
      config->privateKey = (char*)malloc(sizeof(char)*strlen(tmp)+1);
      memcpy(config->privateKey,tmp,strlen(tmp)+1);
      free(tmp);
    }
    
    if(strstr(buffer, "ACTIVE_TLS")){
      char *tmp =  read_str_from_config_line(buffer);
      if(strcmp(tmp,"enable")==0){
	 config->activeTls = 1;
      }
      free(tmp);
    }
	  
    if(strstr(buffer, "HTTP_PORT")){
      config->port =  read_int_config(buffer);
     }
	  
    if(strstr(buffer, "HTTPS_PORT")){
      config->tls_port =  read_int_config(buffer);
    }
	  
    if(strstr(buffer, "NBPROCESS")){
      config->maxProcess =  read_int_config(buffer);
    }
	  
    if(strstr(buffer, "NBTHREAD")){
      config->maxThread =  read_int_config(buffer);
    }
	  
  }
  fclose(fp);
  return config;
}