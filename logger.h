#ifndef _LOGGER_
#define _LOGGER_


typedef enum {
	INFO,
	DEBUG,
	SEVERE,
	FATAL
}typelog;

struct logItem{
	typelog level; /*!< niveau de log */ 
	
	char *message;/*!< message de log */ 

};

typedef struct logItem t_logItem;


struct queingLog{
  t_logItem *front;
  t_logItem *rear;
  t_logItem *queue;
  int size;
};




typedef struct queingLog t_queingLog;




t_queingLog *quelog;
 
void queingLog();

void dequeingLog();

void initlogger();

void logger(typelog tag, const char* message);

const char* tagToString(typelog tag);



#endif