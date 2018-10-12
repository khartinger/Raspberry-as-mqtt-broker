//_____rpi_mqtt_sub2file_log2.c_______________170722-180707_____
// This command line program subscribes all MQTT messages and
// makes one file for each topic with file content is payload.
// Every topic is added to it's log-file named content.yymm.log,
// where yy is the year and mm is the month e.g. test.1803.log
// * optional args: -q = quiet (no output to screen), ip broker
// * A topic rpi_mqtt_sub2file_log2 with payload ? generates a
//   message with same topic and payload with a version number
//   (So you can check, if the program ist running ;)
// * Exit program by pressing <ctrl>c
//
// gcc -o rpi_mqtt_sub2file_log2 rpi_mqtt_sub2file_log2.c utils_file.c -lmosquitto
// (Needs sudo apt-get install libmosquitto-dev !)
//
// Filename    : topic (replacements: ' ' -> '_' and '/' -> '@')
// File content: payload
// Directory1  : _PATH_ (default /var/www/html/mqtt/data/)
// Directory2  : _PATH_LOG_ (default /var/www/html/mqtt/log/)
// MQTT Broker : ip = _HOST_ (default 127.0.0.1)
// 
#include <stdio.h>                // printf, stdin, stdout
#include <stdlib.h>               // exit
#include <unistd.h>               // sleep
#include <string.h>               // sprintf
#include <signal.h>               // SIGINT
#include <time.h>                 // struct tm, time_t
#include "utils_file.h"           // writeline, deletefile
#include <mosquitto.h>            // 

#define  _ANSWER_TOPIC_      "rpi_mqtt_sub2file_log2"
#define  _ANSWER_PAYLOAD_    "20180707"
#define  _PATH_              "/var/www/html/mqtt/data/"
#define  _PATH_LOG_          "/var/www/html/mqtt/log/"

#define  _HOST_              "127.0.0.1"
#define  _HOSTNAME_LEN_      128
#define  _PORT_              1883
#define  _KEEPALIVE_         60
#define  _CLEAN_SESSION_     true

bool prt=true;                    // true=printf, false=quiet
struct mosquitto *mosq = NULL;

//_____add message to log file__________________________________
void add2log(struct mosquitto *mosq, void *userdata,
 const struct mosquitto_message *message)
{
 char filename_[FILENAME_MAX];
 char sNow[18];
 int  ret;
 FILE *fp;
 time_t now_t;
 time(&now_t);
 struct tm* tm_=localtime(&now_t);
 ret=strftime(sNow,5,"%y%m\0",tm_);
 sprintf(filename_,"%s%s.%s.log",_PATH_LOG_,message->topic,sNow);
 ret=strftime(sNow,18,"%d.%m.%y %H:%M:%S\0",tm_);
 //-----open file-----------------------------------------------
 fp=fopen(filename_, "a+");
 if( fp == NULL) return;
 //-----write line to file--------------------------------------
 ret=fputs(sNow, fp);
 if(ret<0) { fclose(fp); return; }
 ret=fputs(" | ", fp);
 if(ret<0) { fclose(fp); return; }
 if(message->topic==NULL) ret=fputs("(null)", fp);
 else ret=fputs(message->topic, fp);
 if(ret<0) { fclose(fp); return; }
 ret=fputs(" | ", fp);
 if(ret<0) { fclose(fp); return; }
 if(message->payload==NULL) ret=fputs("(null)", fp);
 else ret=fputs(message->payload, fp);
 if(ret<0) { fclose(fp); return; }
 ret=fputs("\n", fp);
 //-----close file----------------------------------------------
 ret=fclose(fp);
}

//_____handler for signal SIGxxx (e.g. <ctrl>c)_________________
void my_signal_handler(int signum)
{
 if(signum==SIGINT)
 {
  //-----clean up------------------------------------------------
  mosquitto_destroy(mosq);
  mosquitto_lib_cleanup();
  fprintf(stdout, "\nProgram terminated by <Strg>+C\n");
  exit(0);
 }
 exit(98);
}

//_____print log message________________________________________
void mosq_log_callback(struct mosquitto *mosq,
 void *userdata, int level, const char *str)
{
 switch(level)
 {
  case MOSQ_LOG_DEBUG:
  case MOSQ_LOG_INFO:
  case MOSQ_LOG_NOTICE:
   break;
  case MOSQ_LOG_WARNING:
  case MOSQ_LOG_ERR:
  default:
   fprintf(stderr, "LOG-Message %i: %s\n", level, str);
 }
}

//_____connect info_____________________________________________
void mosq_connect_callback(struct mosquitto *mosq,
 void *userdata, int result)
{
 if(!result)
 { //-----Subscribe all topics on successful connect------------
  mosquitto_subscribe(mosq, NULL, "#", 2);
  //mosquitto_subscribe(mosq, NULL, "$SYS/#", 2); //broker infos
  } else {
  fprintf(stderr, "Connect failed\n");
 }
}

//_____message call back routine________________________________
// parameter will be destroyed after return!
void mosq_msg_callback(struct mosquitto *mosq, void *userdata,
 const struct mosquitto_message *message)
{
 int ret=0;
 char filename[FILENAME_MAX];
 //add2log(mosq, userdata, message);
 //=====PART 1: prepair topic name, show topic | payload========
 if(message->topic==NULL) return;
 //-----replace blank by _, / by @------------------------------
 char* tmp=message->topic;
 while(*tmp) 
 {
  if((*tmp)==' ') *tmp='_';
  if((*tmp)=='/') *tmp='@';
  tmp++;
 }
 add2log(mosq, userdata, message);
 //-----add path------------------------------------------------
 sprintf(filename, "%s%s", _PATH_, message->topic);
 if(prt) printf("%s | %s\n", message->topic, message->payload);
 //=====PART 2: check for empty payload (= topic deleted)=======
 if((message->payloadlen<1)||(message->payload==NULL))
 {
  //-----MQTT: no payload = topic deleted -> delete file--------
  ret=deletefile(filename);
  if(ret!=0)
   fprintf(stderr,"Error %d while deleting file %s\n",ret,filename);
  return;
 }
 //=====PART 3: act on specific payloads========================
 //-----request for answer?-------------------------------------
 if((strcmp(message->topic, _ANSWER_TOPIC_)==0) &&
   (strncmp(message->payload,"?",1)==0))
 {
  //.....publish answer.........................................
  ret=mosquitto_publish(mosq, NULL, _ANSWER_TOPIC_,
   strlen(_ANSWER_PAYLOAD_),_ANSWER_PAYLOAD_,0,0);
  if(ret!=0)
  {
   fprintf(stderr, "Could not send answer. MQTT send error=%i\n",ret);
   return;
  }
  //.....write file.............................................
  ret=writeline(filename, _ANSWER_PAYLOAD_);
  fflush(stdout);
  if(ret!=0)
   fprintf(stdout,"Error %d while writing to file %s\n",ret,filename);
  return;
 }
 //=====PART 4: all other payloads============================== 
 ret=writeline(filename, message->payload);
 fflush(stdout);
 if(ret!=0)
  fprintf(stderr,"Error %d while writing to file %s\n",ret,filename);
}

//_____print help on stdout_____________________________________
void print_help()
{
 fprintf(stdout, "\nUsage  : rpi_mqtt_sub2file_log2 [-q] [ip]\n");
 fprintf(stdout, "         -q ... no output to stdout\n");
 fprintf(stdout, "         ip ... ip of broker (default %s)\n", _HOST_);
 fprintf(stdout, "Purpose: Subscribe all messages from MQTT ");
 fprintf(stdout, "broker and copy them to directory\n");
 fprintf(stdout, "         %s the following way:\n", _PATH_);
 fprintf(stdout, "         filename = topic (replacements: ' ' to '_' and '/' to '@')\n");
 fprintf(stdout, "         file content = payload\n");
 fprintf(stdout, "Additional all messages are added to log files filename.yymm.log\n");
 fprintf(stdout, "         yy ... year, mm ... month\n");
 fprintf(stdout, "Author : Karl Hartinger\n");
 fprintf(stdout, "Version: %s\n",_ANSWER_PAYLOAD_);
 fprintf(stdout, "Needs  : sudo apt-get install libmosquitto-dev\n\n");
}

//_____main program_____________________________________________
int main(int argc, char *argv[])
{
 //char host[_HOSTNAME_LEN_];
 char filename[FILENAME_MAX];
 char host[]=_HOST_;
 char *phost=host;
 int  port=_PORT_;
 bool clean_session=true;
 int  ret;
 strcpy(host, _HOST_);
 //-----Are there any arguments?--------------------------------
 if(argc>3)  { print_help(); return 10; };
 if(argc==1) print_help();
 //-----one arg: -q=no_printf or server name--------------------
 if(argc==2)
 {
  if(argv[1][0]=='-')
  {
   if(argv[1][1]=='h') { print_help(); return(11); }
   if(argv[1][1]=='q') prt=false;
  }
  else
  {
   strcpy(host, argv[1]);
  }
 }
 //-----two args: servername and quiet--------------------------
 if(argc==3)
 {
  if(argv[1][0]=='-')
  {
   //strcpy(host, argv[2]);
   phost=argv[2];
   if(argv[1][1]=='h') { print_help(); return(11); }
   if(argv[1][1]=='q') prt=false;
  }
  else
  {
   //strcpy(host, argv[1]);
   phost=argv[1];
   if(argv[2][0]=='-')
   {
    if(argv[2][1]=='h') { print_help(); return(11); }
    if(argv[2][1]=='q') prt=false;
   }
  }
 }
 //-----prepair exit main loop with <ctrl>c---------------------
 signal(SIGINT, my_signal_handler);
 //-----delete all files in directory---------------------------
 sprintf(filename, "rm -f %s*", _PATH_);
 ret=system(filename); 
 if(ret) fprintf(stderr, "Fehler %d beim Löschen der Dateien\n",ret);
 //-----init network and mqtt-----------------------------------
 mosquitto_lib_init();
 mosq=mosquitto_new(NULL, _CLEAN_SESSION_, NULL);
 if(!mosq)
 {
  fprintf(stderr, "Error: Out of memory.\n");
  return(1);
 }
 mosquitto_log_callback_set(mosq, mosq_log_callback);
 mosquitto_connect_callback_set(mosq, mosq_connect_callback);
 mosquitto_message_callback_set(mosq, mosq_msg_callback);

 if(prt) fprintf(stdout, "Try to connect to mosquitto...\n");
 while(mosquitto_connect(mosq, phost, port, _KEEPALIVE_)!=MOSQ_ERR_SUCCESS)
 {
  if(prt) { fputc('.', stdout); fflush(stdout); }
  sleep(1);
 }
 if(prt) fprintf(stdout, " connected!\n");
 //-----endless loop, terminate program with <ctrl>c------------
 mosquitto_loop_forever(mosq, -1, 1);
 return(99);
}
