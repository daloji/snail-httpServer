#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>
#include "threadpool.h"
#include "config.h"
#include "logger.h"

const char* POINT =".";
const char* DOUBLE_POINT ="..";

const  char *header200 = "HTTP/1.0 200 OK\nServer: Snail Http Server 1.0 \nContent-Type: %s\ncontent-length: %d \n\n";
const char *header400 = "HTTP/1.0 400 Bad Request\nServer: Snail Http Server 1.0\nContent-Type: %s\n\n";
const char *header404 = "HTTP/1.0 404 Not Found\nServer: Snail Http Server 1.0\nContent-Type: %s\n\n";
const char *header500 = "HTTP/1.0 500 Internal server error\nServer:Snail Http Server 1.0\n\n";


//structure reponse HTTP
typedef struct{
   /* code de retour (200/300/400/500/....) */
   int returncode;
   /* fichier de reponse */
   char *filename; 
   
}t_httpResponse;

//structe requet HTTP (GET methode)
typedef struct{
	/* nom du fichier */
	char *filename;
	/* extensions du fichier */
	char *extension;
	/* mime type */
	char *encoding;
	 /* methode GET */
	char *getmethod;
	/*methode  POST*/
	char *postmethod;
	
}t_httpRequest;

t_threadPool *THREADPOOL = NULL;


void releaseAllMemory(){	
  if(THREADPOOL != NULL){
    releaseThreadPool(THREADPOOL);
  }	
}


void signalCallback(int sigid){
  switch(sigid){
    case SIGKILL: 
	releaseAllMemory();
    break;
    case SIGSTOP:
	printf("received SIGSTOP\n");
    break;
    case SIGINT:
	releaseAllMemory();
    break;			
  }
}



/**
 * \fn const char* get_filename_ext(const char *filename)
 * \brief    recupere l'extension d'un fichier
 * \details   recupere l'extension d'un fichier
 *    ex: fichier.js  la fonction renvoi js
 * \param    char    filename nom du fichier .
 * \return    renvoi l'extension du fichier sinoi le type d'encodage est inconnu renvoi NULL
 */
const char* get_filename_ext(const char *filename) {
   const char *dot = strrchr(filename, '.');
   if(!dot || dot == filename) return "";
	return dot + 1;
}


/**
 * \fn const char * getEncodage(const char *filename)
 * \brief    recupere le type d'encodage d'un fichier (mime type)
 * \details  le  type mime est calculé en fonction de l'extension du fichier 
 *       ex: fichier.js  la fonction renvoi text/javascript 
 * \param    char    filename nom du fichier .
 * \return    type d'encodage du fichier si le type d'encodage est inconnu renvoi NULL
 */
const char * getEncodage(const char *filename){
  if(filename == NULL){
    fprintf(stderr, "recuperation encodage impossible extension fichier non definie \n");
    logger(SEVERE,"recuperation encodage impossible extension fichier non definie \n");
    return NULL;
  }
  const char *fileExtension = get_filename_ext(filename);
  if(fileExtension == NULL){
    fprintf(stderr, "encodage extension fichier non definie \n");
    logger(SEVERE,"encodage extension fichier non definie \n");
    return NULL;
  }
  int i=0;
  char *ptrext = extensions[0].ext;
  char *encode = NULL;
  while(ptrext !=NULL){
    if(strcmp(extensions[i].ext,fileExtension)==0){
      encode =extensions[i].filetype;
      break;
    }
    i++;
    ptrext = extensions[i].ext;
  }
  return encode;
}

  

/**
 * \fn void freeStatus(t_httpRequest* req)
 * \brief    liberation memoire de la structure t_httpRequest
 * \details  
 *      
 * \param    t_httpRequest   structure de la requete .
 */
void freeStatus(t_httpRequest* req){
  if(req){
    if(req->filename)
      free(req->filename);
    free(req);
  }
}

/**
 * \fn char * getFileContent(const char* filename)
 * \brief    recupere le contenu d'un fichier (lecture du fichier)
 * \details  le contenu binaire du fichier est renvoyé
 *      
 * \param  char nom du fichier 
 * \return   contenu binaire du fichier renvoi NULL en cas de problème de lecture
 */
char * getFileContent(const char* filename){
  if(filename==NULL){
    fprintf(stderr, "nom du fichier non definie\n");
    logger(SEVERE,"nom du fichier non definie \n");
    return NULL;
  }
  char *buffer = NULL;
  int string_size, read_size;
  FILE *fp = fopen(filename, "rb");
  if(fp==NULL){
    fprintf(stderr, "erreur lors de l'acces au fichier %s \n",filename);
    logger(SEVERE,"erreur lors de l'acces au fichier \n");
    return NULL;
   }
   fseek(fp, 0, SEEK_END);
   string_size = ftell(fp);
   rewind(fp);
   buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
   read_size = fread(buffer, sizeof(char), string_size, fp);
   buffer[string_size] = '\0';
   if (string_size != read_size){
     free(buffer);
     return NULL;
  }
  fclose(fp);
  return  buffer;
  
}

int sendFile(int fd, char *filename) {
  FILE *read;
  if( (read = fopen(filename, "r")) == NULL){
    fprintf(stderr, "Erreur lors de l'ouverture du fichier \n");
    logger(SEVERE,"Erreur lors de l'ouverture du fichier \n");
    exit(EXIT_FAILURE);
   }
   int totalsize;
   struct stat st;
   stat(filename, &st);
   totalsize = st.st_size;
   size_t size = 1;
   char *temp;
   if((temp = malloc(sizeof(char) * size)) == NULL ){
     fprintf(stderr, "Error allocating memory to temp in printFile()\n");
     exit(EXIT_FAILURE);
    }
    int end;
    while( (end = getline( &temp, &size, read)) > 0){
      write(fd, temp, strlen(temp));
    }
    write(fd, "\n",strlen("\n"));
    free(temp);
      
    return totalsize;
 }

/**
 * \brief     liberation de la memoire allouée pour la structure  t_httpRequest (cf #t_httpRequest)
 * \details   la structure  t_httpRequest contient les champs suivantes (filename,extension,getmethod,postmethod)  (cf #t_httpRequest)
 *
 * \param    t_httpRequest    req structure contenant la requete HTTP.
 */  
void freeHttpRequest(t_httpRequest *req){
  if(req != NULL){
    if(req->filename != NULL ){
      free(req->filename);
    }
    if(req->encoding != NULL ){
      free(req->encoding);
    }
    if(req->extension != NULL ){
      free(req->extension);
    }
    if(req->getmethod != NULL){
      //free(req->getmethod);
    }
    if(req->postmethod != NULL){
      //free(req->postmethod);
    }
    free(req);
  }
}

/**
 * \brief    creation de la requete  t_httpRequest  
 * \details   Lorsque  une requete demande un fichier 
 * (ex. GET index.html ) la fonction create la structure t_httpRequest (cf #t_httpRequest)
 *      t_httpRequest->filename = index.html
 * 	t_httpRequest->encoding = text/plain
 * 	t_httpRequest->extension = html
 * 	t_httpRequest->getmethod = GET
 * 
 * \param  char nom du fichier 
 * \return  t_httpRequest (cf #t_httpRequest)  contenu binaire du fichier renvoi NULL en cas de problème de lecture
 */
t_httpRequest * getTypeRequest(const char* filename){
  t_httpRequest *req = NULL;
  if(filename != NULL){
    char *tempfile = calloc(strlen(filename)+1,sizeof(char));
    strcpy(tempfile,filename);
    char *path = strtok(tempfile,"?");
    if(path != NULL){req =(t_httpRequest *) malloc(sizeof(t_httpRequest));
      if(req == NULL){
	fprintf(stderr, "erreur allocation memoire\n");
	exit(EXIT_FAILURE);
      }
      req->filename = malloc(sizeof(char)*strlen(path)+1);
      strcpy(req->filename,path);
      const char *extensionfile = get_filename_ext(req->filename);
      if(extensionfile != NULL){
	req->extension = (char*)malloc((strlen(extensionfile)+1)*sizeof(char));
	strcpy(req->extension,extensionfile);
      }
      const char *encode = getEncodage(req->filename);
      if(encode != NULL){
	req->encoding = (char*)malloc((strlen(encode)+1)*sizeof(char));
	strcpy(req->encoding,encode);
      }else{
	req->encoding = (char*)malloc((strlen("text/plain")+1)*sizeof(char));
	strcpy(req->encoding,"text/plain");
      }
      char *getparam = strstr(filename,"?");
      if(getparam != NULL){
	req->getmethod = malloc(sizeof(char)*strlen(getparam)+1);
	strcpy(req->getmethod,getparam);
      }
    }
  }
	
  return req; 
}

  
/**
 * \fn int sendBinaryFileSocket(int sock,const  t_httpRequest *httpreq)
 * \brief    envoi de la reponse de la requete vers la socket 
 * \details   fonction utilisé pour l'envoi de la reponse d'une requete (utilisé pour les
 * 	fichier binaire (img,mp3,avi,font, ......)
 * 
 * \param  int sock socket  
 * \param  t_httpRequest  *httpreq requete client (cf #t_httpRequest) 
 * \return 
 */
int sendBinaryFileSocket(int sock,const  t_httpRequest *httpreq){
  char responseHeader[MAX_BUFFER];
  if(httpreq == NULL  || httpreq->filename == NULL || httpreq->encoding == NULL){
    // internal error 500
    sprintf(responseHeader, header500);
    sendHttp(sock,responseHeader);
  }else{
    FILE *file = fopen(httpreq->filename, "rb");
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    char send_buffer[size];
    sprintf(responseHeader, header200,httpreq->encoding,size);
    sendHttp(sock,responseHeader);
    while(!feof(file)) {
      fread(send_buffer, 1, sizeof(send_buffer), file);
      write(sock, send_buffer, sizeof(send_buffer));
      bzero(send_buffer, sizeof(send_buffer));
    }
   fclose(file); 
  }
 return 0; 
}

int sendHttp(int fd, void *content){
  int retval = -1;
  if(content != NULL){
    retval = write(fd, content, strlen(content));
  }
  return retval; 
}


char *getMessage(int fd) {
  //file stream
  FILE *sstream;
  // ouverture du file stream et erreur en cas d'echec d'ouverture
  if( (sstream = fdopen(fd, "r")) == NULL){
    fprintf(stderr, "Error opening file descriptor in getMessage()\n");
    exit(EXIT_FAILURE);
  }
  // Size variable utilisée pour recuprer la taille de chaque bloc lu (taille 1 octet) bloc inital
  size_t size = 1;
  char *block = NULL;
  // Allocation memoire  variable bloc pour chaque block memoire 
  if( (block = malloc(sizeof(char) * size)) == NULL ){
    fprintf(stderr, "Error allocating memory to block in getMessage\n");
    exit(EXIT_FAILURE);
  }
  // initialistion chaine null   
  *block = '\0';
  // Allocation block memoire 
  char *tmp = NULL;
  if( (tmp = malloc(sizeof(char) * size)) == NULL ){
    fprintf(stderr, "Error allocating memory to tmp in getMessage\n");
    exit(EXIT_FAILURE);
  }
  // initialistion chaine null 
  *tmp = '\0';
  // taille du flux lu
  int oldsize = 1;
  // recuperation de chaque line du flux
  while(getline( &tmp, &size, sstream) > 0){
    // si le flux lu contient un retour chariot (\n) et une sepace (fin de ligne flux)
    if( strcmp(tmp, "\r\n") == 0){
      break;
    }
    // redimensionement du block
    block = realloc(block, size+oldsize);
    //memorisation de la taille des blocs lu
    oldsize += size;
    //concatenation du chaque bloc lu
    strcat(block, tmp);
  }
  // liberation memoire
  free(tmp);
  // Return header HTTP
  return block;
}


// Extracts the filename needed from a GET request and adds public_html to the front of it
char * getFileName(char* directory,char* header){
  if(directory == NULL){
    fprintf(stderr, "repertoire racine NULL\n");
    logger(FATAL,"repertoire racine NULL \n");
    return NULL;
  }
  if(header == NULL){
    fprintf(stderr, "header de la requete NULL\n");
    logger(FATAL,"header de la requete NULL \n");
    return NULL;
  }
  char * file = NULL;
  // Allocate some memory for the filename and check it went OK
  if( (file = malloc(sizeof(char) * strlen(header))) == NULL){
    fprintf(stderr, "Erreur lors de l'allocation memoire  getFileName()\n");
    logger(FATAL,"Erreur lors de l'allocation memoire  getFileName() \n");
    exit(EXIT_FAILURE);
  }
  sscanf(header, "GET %s HTTP/1.1", file);
  char *base = NULL;
  if( (base = malloc(sizeof(char) * (strlen(file) + strlen(directory)+1))) == NULL){
    fprintf(stderr, "Erreur lors de l'allocation memoire  getFileName()\n");
    logger(FATAL,"Erreur lors de l'allocation memoire  getFileName() \n");
    exit(EXIT_FAILURE);
  }
  strcpy(base, directory);
  strcat(base, file);
  free(file); 
  return base;
}

/**
 * \brief    fonction thread qui prend en charge chaque requette 
 * \details  chaque thread en attente  pointe vers cette fonction elle prend en charge chaque requette
 *      
 * \param  void * (t_argumentThread)  (cf #t_argumentThread) argument contient les arguments de la fonction passer par le threadr 
*/
void  *processRequest(void *argument){
  t_argumentThread *arg = (t_argumentThread *)argument;
  if(arg->wwwDirectory == NULL){
    fprintf(stderr, "pointeur de fonction thread repetoire de travail non defini\n");
    logger(FATAL,"pointeur de fonction thread repetoire de travail non defini \n");
    exit(EXIT_FAILURE); 
  }
  while(1){
    struct sockaddr_in addr;
    int len = sizeof(addr);
    //printListFile(list);
    int listend = arg->socketFd;
    //int con = accept(listend, &addr, &len);
    int con = accept(listend, NULL, NULL);
    //printf("Connection: %s:%d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));	
    char * header = getMessage(con);
    t_httpResponse *httpresponse=NULL;
    t_httpRequest *httpreq=NULL;
    char responseHeader[MAX_BUFFER];
    char *content = NULL;
    char *filename = getFileName(arg->wwwDirectory,header);
    if(filename != NULL){
      httpreq = getTypeRequest(filename);
      if(httpreq != NULL && httpreq->filename != NULL){
	if(strcmp(httpreq->extension,"woff2")==0 ||strcmp(httpreq->extension,"jpg") == 0 ){
	  sendBinaryFileSocket(con,httpreq);
	}else{
	  content = getFileContent(httpreq->filename);
	}
      }
    }
    if(httpreq != NULL && content !=NULL){
      if(httpreq->encoding != NULL){
	sprintf(responseHeader, header200,httpreq->encoding,strlen(content));
      }
    }
    if(content !=NULL && httpreq != NULL){
      sendHttp(con,responseHeader);
      sendHttp(con,content);
    }
    freeHttpRequest(httpreq);
    close(con);
  }
  free(arg->threadname);
  free(arg);
}




int main(int argc, char *argv[]){
  int listenfd = 0, connfd = 0 ,i = 0,pid;
  struct sockaddr_in serv_addr; 
  char sendBuff[1025];
  pthread_t threadLogger;
  initlogger();  
  t_config *config= readconfig("server.conf");
  if(config == NULL){
    logger(FATAL,"erreur lors de la lecture de la configuration\n");
    exit(-1);
   }
   if(config->port<0){
     logger(FATAL,"port server non defini\n");
     exit(-1);
   }
    
    
   if(pthread_create(&threadLogger, NULL, &loggMessageTofile, config->filelog) == -1) {
     fprintf(stderr, "Echec de la creation thread Logger -> allocation memoire impossible\n");
     exit(EXIT_FAILURE);
   }

   listenfd = socket(AF_INET, SOCK_STREAM, 0);
   memset(&serv_addr, '0', sizeof(serv_addr));
   memset(sendBuff, '0', sizeof(sendBuff)); 
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   serv_addr.sin_port = htons(config->port); 
   bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
   listen(listenfd, 10); 
   t_argumentThread *argument = (t_argumentThread *) malloc(sizeof(t_argumentThread));
   if(argument == NULL){
      fprintf(stderr, "Echec de la creation argument thread -> allocation memoire impossible\n");
      logger(FATAL,"Echec de la creation argument thread -> allocation memoire impossible\n");
      exit(-1);
   }
   argument->poolId=getpid();
   argument->socketFd=listenfd;
   if(config->wwwDirectory ==NULL){
      logger(FATAL,"repertoire source non configuré dans le fichier de configuration\n");
      exit(-1);
   }
	  
   argument->wwwDirectory=config->wwwDirectory;
   for(i=0;i<config->maxProcess;i++){
      //prefork mode     
      pid=fork();
      if (pid == -1){
	fprintf(stderr, "echec lors de l'appel a fork\n");
	logger(FATAL,"echec lors de l'appel a fork \n");
	exit(EXIT_FAILURE);
      }
      if(pid==0){  
	THREADPOOL = createThreadPool(config->maxThread,(void *)processRequest,(void *)argument);
	}else{
	  if (waitpid(pid, NULL, 0) < 0) {
	    logger(FATAL,"echec fatal waitpid \n");
	    break;
	  }
	} 
    }
	  
    signal(SIGINT, signalCallback);
    signal(SIGKILL, signalCallback);
    signal(SIGSTOP, signalCallback);
    pthread_exit(NULL);
    waitpid(WAIT_ANY, NULL, WNOHANG);
    exit(1); 
  }