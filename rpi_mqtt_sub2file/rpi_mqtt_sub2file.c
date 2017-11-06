//_____rpi_mqtt_sub2file.c____________________170722-171106_____
// Subscribe all MQTT messages and copy them to files.
// (One messsage = one file)
// MQTT Broker : ip = _HOST_ (default 192.168.1.1)
// Directory   : _PATH_ (default /var/www/html/mqtt/data/)
// Filename    : topic (replacements: ' ' -> '_' and '/' -> '&')
// File content: payload
// 
// gcc -o rpi_mqtt_sub2file rpi_mqtt_sub2file.c utils_file.c -lmosquitto
// needs sudo apt-get install libmosquitto-dev

#include <stdio.h>                // printf, stdin, stdout
#include <mosquitto.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils_file.h"
#define  _HOST_         "192.168.1.1"
#define  _PATH_         "/var/www/html/mqtt/data/"
bool prt=true;                    // true=printf, false=quiet

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
 int ret;
 char filename[FILENAME_MAX];
 //-----replace blank by _, / by &------------------------------
 char* tmp=message->topic;
 while(*tmp) 
 {
  if((*tmp)==' ') *tmp='_';
  if((*tmp)=='/') *tmp='&';
  tmp++;
 }
 //-----add path (no extension ;)-------------------------------
 sprintf(filename, "%s%s", _PATH_, message->topic);
 if(prt) printf("=> %s | %s\n", filename, message->payload);
 if(message->payloadlen)
  ret=writeline(filename, message->payload);
 else
  ret=writeline(filename, "");
 fflush(stdout);
 if(ret!=0)
  printf("Fehler %d beim Schreiben in die Datei %s\n",ret,filename);
}

//_____print help on stdout_____________________________________
void print_help()
{
 fprintf(stdout, "\nSubscribe all messages from MQTT broker and\n");
 fprintf(stdout, "copy them to directory %s\n", _PATH_);
 fprintf(stdout, "        filename = topic, file content = payload\n");
 fprintf(stdout, "        topic replacements: ' ' -> '_' and '/' -> '&'\n");
 fprintf(stdout, "Author: Karl Hartinger, 6.11.2017\n");
 fprintf(stdout, "Usage : rpi_mqtt_sub2file [-q] [ip]\n");
 fprintf(stdout, "        -q ... no output to stdout\n");
 fprintf(stdout, "        ip ... ip of broker (default %s)\n", _HOST_);
 fprintf(stdout, "Needs sudo apt-get install libmosquitto-dev\n\n");
}

//_____main program_____________________________________________
int main(int argc, char *argv[])
{
 char host[128];
 int  port=1883;
 int  keepalive=60;
 bool clean_session=true;
 struct mosquitto *mosq = NULL;
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
   strcpy(host, argv[2]);
   if(argv[1][1]=='h') { print_help(); return(11); }
   if(argv[1][1]=='q') prt=false;
  }
  else
  {
   strcpy(host, argv[1]);
   if(argv[2][0]=='-')
   {
    if(argv[2][1]=='h') { print_help(); return(11); }
    if(argv[2][1]=='q') prt=false;
   }
  }
 }
 //-----init network and mqtt-----------------------------------
 mosquitto_lib_init();
 mosq=mosquitto_new(NULL, clean_session, NULL);
 if(!mosq)
 {
  fprintf(stderr, "Error: Out of memory.\n");
  return(1);
 }
 mosquitto_log_callback_set(mosq, mosq_log_callback);
 mosquitto_connect_callback_set(mosq, mosq_connect_callback);
 mosquitto_message_callback_set(mosq, mosq_msg_callback);

 if(prt) printf("Try to connect to mosquitto...\n");
 while(mosquitto_connect(mosq, host, port, keepalive)!=MOSQ_ERR_SUCCESS)
 {
  if(prt) { fputc('.', stdout); fflush(stdout); }
  sleep(1);
 }
 if(prt) printf(" connected!\n");
 //-----endless loop-------------------------------------------- 
 mosquitto_loop_forever(mosq, -1, 1);
 //-----clean up------------------------------------------------
 mosquitto_destroy(mosq);
 mosquitto_lib_cleanup();
 return 0;
}
