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

struct config{
   char *wwwDirectory;
   char *filelog;
   int port;
   int maxThread;
   int maxProcess;
};

struct fileRequest{
    char *filename;
    char* filecontent;
    int sizeFile;
};

struct extension{
   char *ext;
   char *filetype;
};

typedef enum {
	INFO,
	DEBUG,
	SEVERE,
	FATAL
}typelog;

t_config * readconfig(char *filename);

void logger(typelog tag, const char* message);

const char* tagToString(typelog tag);


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