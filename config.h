#ifndef _C0NFIG_
#define _C0NFIG_
#define DELIM "="

#define FILENAME "server.conf"

static const char *DIRECTORY="DIRECTORY";
static const char *PORT ="PORT";
static const char *NBPROCESS ="NBPROCESS";
static const char *NBTHREAD ="NBTHREAD";
static const char *LOGFILE ="LOGFILE";
static const int MAX_BUFFER = 1024;
static int OK=200;

typedef struct config t_config;

typedef struct fileRequest t_fileRequest;

typedef struct extension t_extension;

/**
 *  \brief Structure representant la configuration du serveur
 *  \details la structure est valorisée par la fonction de lecture de la configuration
 */
struct config{
   char *wwwDirectory; /*!< repertoire de travail qui contient les sources de votre site web (ex : index.html)*/ 
   
   char *filelog; /*!< fichier de log du serveur*/ 
   
   char *certFile;  /*!< fichier  certificat */
   
   int port; /*!< port d'ecoute du serveur */ 
   
   int tls_port; /*!< port d'ecoute  du serveur  ssl */ 
   
   int maxThread;/*!< nombre de thread  qui sont crée dans chaque processus */ 
   
   int maxProcess;/*!< nombre de processus crée par fork() */ 
};

struct fileRequest{
    char *filename;
    char* filecontent;
    int sizeFile;
};


/**
 *  \brief Structure representant  la liste des mimes types gerer par le serveur
 *  \details la liste des structure est statique
 */
struct extension{
   char *ext; /*!< type extension */ 
   
   char *filetype;/*!< mime type */ 
};



t_config * readconfig(char *filename);


FILE *FILELOG;
//list des mimes types
static const  t_extension extensions [] = {
	  {"gif", "image/gif" },  
	  {"jpg", "image/jpeg"}, 
	  {"jpeg","image/jpeg"},
	  {"png", "image/png" },  
	  {"zip", "image/zip" },  
	  {"gz",  "image/gz"  },  
	  {"tar", "image/tar" },  
	  {"htm", "text/html" },  
	  {"html","text/html" },  
	  {"php", "image/php" },  
	  {"cgi", "text/cgi"  },  
	  {"asp","text/asp"   },  
	  {"jsp", "image/jsp" },  
	  {"xml", "text/xml"  },  
	  {"js","text/javascript"},
	  {"ttf","application/x-font-ttf"},
	  {"woff2","application/font-woff2"},
	  {"woff","application/font-woff"},
	  {"css","text/css"},
	  {NULL,NULL} };

#endif