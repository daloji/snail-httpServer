#ifndef _LOGGER_
#define _LOGGER_


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




typedef struct queingLog t_queingLog;


t_queingLog *QUEUELOG;
 
t_logItem* createQueueLog(const char* message);
 
void queingLog(t_logItem *log);

char* dequeingLog();

void initlogger();

void logger(typelog tag, const char* message);

const char* tagToString(typelog tag);



#endif