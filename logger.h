#ifndef _LOGGER_
#define _LOGGER_
#define MAX_BUFFERS_LINE 10
#define MAX_BUFFERS 256
#define MSGPERM 0600 


struct logmessage{
  long  type;                   // impose, type du msg
  pid_t pidEmetteur;
  char message[MAX_BUFFERS];  
};

typedef struct logmessage t_logmessage;

typedef enum {
	INFO,
	DEBUG,
	SEVERE,
	FATAL
}typelog;


typedef struct logItem t_logItem;

struct logItem{
    char *message;/*!< message de log */ 
    t_logItem * next;
};


struct queingLog{
  t_logItem *front;
  t_logItem *rear;
  int size;
};


int shmid;

typedef struct queingLog t_queingLog;

void  *loggMessageTofile(void *argument);


void  *loggMessageTofile(void *argument);

t_queingLog *QUEUELOG;
 
void appendQueueLog(t_logItem* list,char* message);
 
void queingLog(t_logItem *log);

char* dequeingLog();

void initlogger();

void addLog(char *message);

void logger(typelog tag, const char* message);

const char* tagToString(typelog tag);

int getSizeQueue();



#endif