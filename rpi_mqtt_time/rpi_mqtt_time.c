// ___rpi_mqtt_time.c_____________171119-200125___K_Hartinger___
// This command line program subscribes MQTT topic 'getTime'
// and publishes topic 'brokertime' with message YYYYmmdd HHMMSS
// * when 'version' is received, program sends its version number
// * when 'end' is received, the program ends.
// * Exit program by pressing <ctrl>c
// gcc rpi_mqtt_time.c -o rpi_mqtt_time -lmosquitto
// (Needs sudo apt-get install libmosquitto-dev !)
// LIB-Info: https://mosquitto.org/api/files/mosquitto-h.html
#include <stdio.h>                // fprintf, stdout
#include <stdlib.h>               // exit
#include <unistd.h>               // sleep
#include <signal.h>               // SIGINT
#include <time.h>                 // struct tm, time_t
#include <string.h>               // sprintf
#include <mosquitto.h>            // 

#define  _TOPIC_IN_          "getTime"
#define  _TOPIC_OUT_         "brokertime"
#define  _END_               "end"
#define  _VERSION_           "Version 20200125 (20180708)"

#define  _HOST_              "127.0.0.1"
#define  _HOSTNAME_LEN_      256
#define  _PORT_              1883
#define  _KEEPALIVE_         60
#define  _CLEAN_SESSION_     true

bool prt=true;                    // true=printf, false=quiet
struct mosquitto *mosq = NULL;
char sNow[18];

//_____handler for signal SIGxxx (e.g. <ctrl>c)_________________
void my_signal_handler(int signum)
{
 if(signum==SIGINT)
 {
  //-----clean up------------------------------------------------
  mosquitto_destroy(mosq);
  mosquitto_lib_cleanup();
  if(prt) fprintf(stdout, "\nProgram terminated by <Strg>+C\n");
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
   if(prt) fprintf(stderr, "LOG-Message %i: %s\n", level, str);
 }
}

//_____connect info_____________________________________________
void mosq_connect_callback(struct mosquitto *mosq,
 void *userdata, int result)
{
 if(!result)
 { //-----Subscribe topic on successful connect-----------------
  mosquitto_subscribe(mosq,NULL,_TOPIC_IN_,2);  //2=QOS=exact 1x
  } else {
  if(prt) fprintf(stderr, "Connect failed\n");
 }
}

//_____message call back routine________________________________
// called by every subscribed message
// parameter will be destroyed after return!
void mosq_msg_callback(struct mosquitto *mosq, void *userdata,
 const struct mosquitto_message *message)
{
 int ret;
 //=====PART 1: prepair topic name, show topic | payload========
 if(message->topic==NULL) return;
 if(strlen(message->topic)<1) return;
 if(prt) printf("%s | %s\n", message->topic, message->payload);
 //=====PART 2: check for empty payload=========================
 if((message->payloadlen<1)||(message->payload==NULL))
 {
  if(prt) printf("rpi_mqtt_time: No Payload\n");
  return;
 }
 //=====PART 3: act on specific payloads========================
 //-----if payload 'version': send version of this program------
 if((strcmp(message->payload, "version")==0) ||
    (strcmp(message->payload, "Version")==0))
 {
  //-----publish answer-----------------------------------------
  ret=mosquitto_publish(mosq, NULL,_TOPIC_OUT_,
  strlen(_VERSION_), _VERSION_, 0, 0);
  if(ret!=0)
  {
   if(prt) fprintf(stderr, "Could not send answer. MQTT send error=%i\n",ret);
  }
  return;
 }
 //-----if payload 'end': terminate this program----------------
 if(strcmp(message->payload, _END_)==0)
 {
  //.....clean up...............................................
  mosquitto_destroy(mosq);
  mosquitto_lib_cleanup();
  if(prt) fprintf(stdout, "Program terminated by payload 'end'.\n");
  exit(0);
 }
 //-----for any other payload-----------------------------------
  time_t now_t;
  time(&now_t);
  struct tm* tm_=localtime(&now_t);
  strftime(sNow,16,"%Y%m%d %H%M%S\0",tm_);
  //strftime(sNow,18,"%d.%m.%y %H:%M:%S\0",tm_);
  if(prt) fprintf(stdout, "%s\n",sNow);
  //-----publish answer------------------------------------------
  ret=mosquitto_publish(mosq, NULL,_TOPIC_OUT_,
   strlen(sNow), sNow, 0, 0);
  if(ret!=0)
  {
   if(prt) fprintf(stderr, "Could not send answer. MQTT send error=%i\n",ret);
  }

 return;
}

//_____print help on stdout_____________________________________
void print_help()
{
 fprintf(stdout, "\nUsage  : rpi_mqtt_time [-q]\n");
 fprintf(stdout, "         -q ... no output to stdout, stderr\n");
 fprintf(stdout, "Purpose: Subscribe topic 'getTime' from MQTT ");
 fprintf(stdout, "broker and \n");
 fprintf(stdout, "         answer with topic 'brokertime', ");
 fprintf(stdout, "message 'YYYYmmdd HHMMSS'\n");
 fprintf(stdout, "Author : Karl Hartinger\n");
 fprintf(stdout, "Version: %s\n",_VERSION_);
 fprintf(stdout, "Needs  : sudo apt-get install libmosquitto-dev\n\n");
}

//_____main program_____________________________________________
int main(int argc, char *argv[])
{ 
 bool doHelp_=true;
 char host[_HOSTNAME_LEN_];
 strcpy(host, _HOST_);
 
 //-----Are there any arguments?--------------------------------
 if(argc>1)
 {
  if(argc==2)
  {
   if(argv[1][0]=='-')
   {
    if(argv[1][1]=='q') { prt=false; doHelp_=false; }
   }
  }
  if(doHelp_) { print_help(); return 10; };
 }
 //-----init signal handler-------------------------------------
 signal(SIGINT, my_signal_handler);
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
 while(mosquitto_connect(mosq, host, _PORT_, _KEEPALIVE_)!=MOSQ_ERR_SUCCESS)
 {
  if(prt) { fputc('.', stdout); fflush(stdout); }
  sleep(1);
 }
 if(prt) fprintf(stdout,"Connected: Waiting for topic %s\n",_TOPIC_IN_);
 //-----endless loop, terminate program with <ctrl>c------------
 mosquitto_loop_forever(mosq, -1, 1);
 return(99);
}