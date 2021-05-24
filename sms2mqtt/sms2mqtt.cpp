//___sms2mqtt.cpp_____________________________Karl Hartinger____
// g++ sms2mqtt.cpp C_Gsm.cpp C_X232.cpp C_Conf_s2m.cpp C_mosquitto.hpp -lmosquitto -pthread  -o sms2mqtt -std=c++17
// Convert SMS to MQTT messages and vice versa.
// (1) Receive an SMS and send content as mqtt message
//     SMS format: "-t topic -m payload"
// (2) Receive a mqtt message and send payload as SMS
//     payload format: +43680xxx sms-text
// The program checks whether the specified phone number is 
//   authorized to send or receive an SMS.
// Furthermore, the MQTT receive topic is taken from the
//   configuration file.
// All configuration data is located in the file sms2mqtt.conf.
// 
// Hardware: (1) Raspberry Pi 3 or 4
//           (2) GSM-Modul SIM800C at serial port
// Created by Karl Hartinger, May 23, 2021.
// Changes:
// 21-05-23 New
// Released into the public domain.
#include <csignal>                     // SIGTERM, SIGINT, kill, signal
#include <thread>                      // thread
#include "C_Gsm.h"                     // Gsm module, SMS
#include "C_X232.h"                    // serial interface
#include "C_Conf_s2m.h"                // class Conf
#include "myMosq.h"                    // MyMosquitto, g_myMosq

#define  _VERSION_           "2021-05-23"
#define  _DEBUG_             false
#define  _PATH_              "./"
#define  _FILE_CONF_         "sms2mqtt.conf"
#define  _PFILE_CONF_        _PATH_ _FILE_CONF_
#define  _HOST_              "127.0.0.1"
#define  _PORT_              1883
#define  _KEEPALIVE_         60

Conf  conf=Conf();                     // all configuration data
bool  g_smsBusy;                       // true while sending sms
int   g_endByXXX=-1;                   // reason for prog end
pid_t g_pid_main=-1;                   // main process id to kill

//_______function delaration____________________________________
void terminate_program();
void my_signal_handler(int signum);
std::string getDateTime();
int  parseSmsIn( std::string smsText, std::string& topicIn, std::string& payloadIn, bool& retainIn);
void fSendSms(std::string phone, std::string text);

//_______terminate programm_____________________________________
void terminate_program()
{
 std::string s1;
 //------string with reason for terminating---------------------
 switch(g_endByXXX)
 {
  case 1: s1="Program terminated by <Strg>+C";
   break;
  case 2: s1="Program terminated by MQTT";
   break;
  case 3: s1="Program terminated by SMS";
   break;
  case 4: s1="Program terminated by 'mqtt loop #1'";
   break;
  case 5: s1="Program terminated by 'periodic loop #2'";
   break;
  default: s1=g_endByXXX;
   s1="Program terminated by unknown reason #"+s1;
   break;
 }
 if(conf.getAddTime()) s1=" ("+getDateTime()+") - "+s1;
 else s1="";
 //------send end MQTT message----------------------------------
 try {
  if(conf.getEndTopic().length()>1) {
   std::string s2=conf.getEndPayload()+s1;
   g_myMosq.publish(conf.getEndTopic(),s2);
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
 } catch(std::string& error) {
  fprintf(stderr,"Error while publishing answer: %s\n",error);
 }
 //------send end SMS-------------------------------------------
  if(conf.getEndSmsPhone().length()>2) {
  std::string s2=conf.getEndSmsText()+s1;
  std::thread threadSms(fSendSms, conf.getEndSmsPhone(), s2);
  threadSms.join();                    // wait for thread ended
 }
 //------print reason for terminating---------------------------
 fprintf(stdout, ("\n"+s1+"\n").c_str());
 //------clean up-----------------------------------------------
 kill(g_pid_main, SIGTERM);
 //g_myMosq.cleanup();                 //done by destructor !!
 exit(g_endByXXX);
}

//_______handler for signal SIGxxx (e.g. <ctrl>c)_______________
void my_signal_handler(int signum)
{
 if(signum==SIGINT) { g_endByXXX=1; terminate_program(); }
 exit(98);
}

//_______return actual system date and time_____________________
std::string getDateTime()
{
 char caNow[20];
 time_t now_secs;
 time(&now_secs);
 struct tm* time_=localtime(&now_secs);
 strftime(caNow,20,"%d.%m.%Y %H:%M:%S\0",time_);
 std::string s(caNow);
 return s;
}

//_______split SMS to MQTT message parts________________________
// return <0: error, ().....In values unchanged)
// return  1: only plain text (.....In values changed)
// return  2: correct MQTT message (.....In values changed)
int parseSmsIn( std::string smsText, std::string& topicIn, 
   std::string& payloadIn, bool& retainIn)
{
 bool bRetain=false;
 int ixt, ixm, ixr;
 int lent, lenm, lenx;
 int lenText=smsText.length();
 std::string key1, val1;
 //-----------Parse SMS (-t topic -m message)-------------------
 ixt=smsText.find("-t");                    // search for -t
 if(ixt==std::string::npos)
 {//----------NOT found -t--------------------------------------
  ixm=smsText.find("-m");                   // search for -m
  ixr=smsText.find("-r");                   // search for -r
  if(ixm==std::string::npos && ixr==std::string::npos)
  {//---------neither -t nor -m nor -r--------------------------
   conf.delExtBlank(smsText);               // 
   if(smsText.length()>0)
   {//--------plain text without -X is ok-----------------------
    topicIn=_CONF_S2M_TOPICPUB_;
    payloadIn=smsText;
    retainIn=true;
    return 1;                               // only plain text
   } // end plain text without -X is ok
  } // end neither -t nor -m nor -r
  return -1;                                // wrong -X
 } // end NOT found -t
 //-----------found -t------------------------------------------
 ixm=smsText.find("-m");                    // search for -m
 if(ixm==std::string::npos) return -2;      // wrong -t -m
 ixr=smsText.find("-r");                    // search for -r
 if(ixr==std::string::npos) ixr=lenText;    // string end
 else bRetain=true;                         // -r found
 //-----------find topic length---------------------------------
 lent=lenText-ixt-2;
 if(ixr>ixt) {
  lenx=ixr-ixt-2; 
  if(lenx<lent) lent=lenx;
 }
 if(ixm>ixt) {
  lenx=ixm-ixt-2;
  if(lenx<lent) lent=lenx;
 }
 key1=smsText.substr(ixt+2, lent);
 conf.delExtBlank(key1);                    // 
 if(key1.length()<1) return -3;             // wrong topic
 //-----------find payload length-------------------------------
 lenm=lenText-ixm-2;
 if(ixr>ixm) {
  lenx=ixr-ixm-2; 
  if(lenx<lenm) lenm=lenx;
 }
 if(ixt>ixm) {
  lenx=ixt-ixm-2;
  if(lenx<lenm) lenm=lenx;
 }
 val1=smsText.substr(ixm+2, lenm);
 conf.delExtBlank(val1);                    // 
 if(val1.length()<1) return -4;             // wrong payload
 //-----------topic and payload ok------------------------------
 topicIn=key1;
 payloadIn=val1;
 retainIn=bRetain;
 return 2;                                  // correct message
}

//_______thread function "send sms"_____________________________
void fSendSms(std::string phone, std::string text)
{
 if(_DEBUG_) std::cout<<"{fSendSms thread started}" <<std::endl;
 std::string s1;
 bool   bRetain=false;
 bool   bRet;
 //-----------wait for modem to be ready or 180secs-------------
 int watchdog1=120;
 while(g_smsBusy && watchdog1>0)
 {
  watchdog1--;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
 }
 //-----------start sms sending procedure-----------------------
 g_smsBusy=true;                            // start sending
 if(watchdog1<1) {
  s1="SMS NOT sent #1: timeout ("+phone+": "+text+")";
 } else {
  if(!conf.isAuthSmsTo(phone)) {
   s1="SMS NOT sent #2: Unauthorized number ("+phone+")";
  } else {
   Gsm gsm_=Gsm(conf.getDevice());
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   if(!gsm_.isModule()) {
     s1="SMS NOT sent #3: "+gsm_.getsStatus()+" ("+phone+": "+text+")";
   } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if(!gsm_.begin()) {
     s1="SMS NOT sent #4: "+gsm_.getsStatus()+" ("+phone+": "+text+")";
    } else {
     std::this_thread::sleep_for(std::chrono::milliseconds(100));
     if(gsm_.sendSms(phone, text)) {
      s1="SMS sent ("+phone+": "+text+")";
     } else {
      s1="SMS NOT sent #5: "+gsm_.getsStatus()+" ("+phone+": "+text+")";
     }
    }
   }
  }
 }
 try {
  g_myMosq.publish(conf.getTopicPub(), s1, bRetain);
 } catch(std::string& error) {
  fprintf(stderr,"Error \"%s\" while publishing SMS (%s)\n",error, s1);
 }
 g_smsBusy=false;                           // enable next send
}

//_____react to mqtt messages___________________________________
// (1) Add payload to log file
// (2) Update data file
// (3) Act on topic "rpi_mqttcontrol1" with 
//     payload "?" or "version" or "end"
// (4) call function myMQTTonMessage
void MyMosquitto::onMessage(std::string topic, std::string payload)
{
 //======PART 1: plausibility check topic name, payload=========
 if(topic.length()<1) return;
 if(payload.length()<1) return;
 if(_DEBUG_) std::cout<<"MyMosquitto::onMessage: Topic: "<<topic<<", Payload: "<<payload<<std::endl; 
 //======PART 2: check correct topic============================
 int len1=conf.getTopicSub().length();
 int len2=topic.length();
 if(len2<len1) return;
 if(topic.substr(0,len1)!=conf.getTopicSub()) return;
 //======PART 3: if payload 'end': terminate this program=======
 if(payload==conf.getCmdEnd()) {
  g_endByXXX=2;                   // program end by mqtt command
  terminate_loop();               // stopp mqtt "endless" loop
 }
 //======PART4: other commands==================================
 if(payload==conf.getCmdVersion()) {
   g_myMosq.publish(conf.getTopicPub(), _VERSION_, false);
   return;
 }
 if(payload==conf.getCmdReload()) 
 {//-----read config file, send success message-----------------
  std::string s1="Config file read: ";
  conf.initConf();
  if(conf.readFile(_PFILE_CONF_))
   g_myMosq.publish(conf.getTopicPub(), s1+"OK", false);
  else
   g_myMosq.publish(conf.getTopicPub(), s1+"ERROR", false);
  return;
 }
 //======PART 5: prepare sms sending============================
 std::string phone;
 std::string text;
 conf.split2String(payload,phone,text,' ');
 if(!conf.isAuthSmsTo(phone)) return;
 std::thread threadSms(fSendSms, phone, text);
 threadSms.detach();
}

//_____periodic actions (a parallel thread)_____________________
// waiting for sms
void myMQTTperiodicExec(std::string param)
{
 bool bDoPeriodic=true;
 bool bRet;                                 // bool return
 bool bRetain;                              // true=retain message
 SSMS smsIn;                                // incomming SMS
 Gsm gsm2=Gsm(conf.getDevice());
 g_endByXXX=5;                              // prog end by loop #2
 //-----------endless loop #2: waiting for incoming SMS---------
 while(bDoPeriodic)
 {
  //==========PART 1: Has an SMS arrived?=======================
  if(gsm2.isSms1())
  {//---------isSMS1--------------------------------------------
   bRet=gsm2.readSms1(smsIn);
   gsm2.deleteSms1();
   if(bRet)
   {//--------read sms 1: success-------------------------------
    if(conf.isAuthSmsFrom(smsIn.phone))
    {//-------SMS comes from an authorized sender---------------
     std::string topicIn, payloadIn;
     bool retainIn;
     int  iRet;
     iRet=parseSmsIn(smsIn.text, topicIn, payloadIn, retainIn);
     if(iRet==1)
     {//------SMS is plain text (command or credit answer)------
      if(smsIn.text==conf.getCmdEnd()) 
      {//.....sms command end...................................
       bDoPeriodic=false;
       g_endByXXX=3;                        // prog end by SMS
       terminate_program();
      } else 
      {
       if(smsIn.text==conf.getCmdVersion()) 
       {//....SMS command version...............................
        std::thread threadSms(fSendSms, smsIn.phone, _VERSION_);
        threadSms.detach();
       } else 
       {//....other plain text -> publish it....................
        g_myMosq.publish(topicIn, payloadIn, retainIn);
       }
      }
     }
     if(iRet==2)
     {//------SMS is a correct MQTT message: publish it---------
       g_myMosq.publish(topicIn, payloadIn, retainIn);
     }
    } // end isAuthSmsFrom
   } // end read sms 1: success
  } // end isSms1
 
  //==========PART 2: do something else=========================
  // ...
  //==========PART 3: wait a little bit=========================
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
 }; //end while
 //-----------end of "endless loop #2"--------------------------
 std::this_thread::sleep_for(std::chrono::milliseconds(1000));
 g_endByXXX=4;
 terminate_program();
}

//_______MAIN___________________________________________________
int main(int argc, char **argv)
{
 int  ret=0;                           // return value
 bool bRet=false;                      // bool return value
 bool doLoop=true;                     // endless main loop
 char host[]=_HOST_;                   // host=localhost
 char *phost=host;                     // pointer to hostname
 int  port=_PORT_;                     // mqtt port
 std::string s1;                       // help string
 //------check argument(s)--------------------------------------
 if(argc==2)
 {
  if(argv[1][0]=='-')
  {
   switch(argv[1][1])
   {
    case 'h': ret=-1; break;
    default: break;
   }
  }
 }
 if(argc>2) ret=-3; //SST_TOO_MANY_ARGS;
 //------if params not ok: help text----------------------------
 if(ret!=0)
 {
  printf("Raspberry Pi: Convert SMS to MQTT messages and vice versa.\n");
  printf("              Needs a GSM module\n");
  printf("Author:   Karl Hartinger, 23.5.2021\n");
  printf("Usage:    sms2mqtt [-h]\n");
  printf("Params:   -h ...... help\n");
  printf("Config:   file \"sms2mqtt.conf\"\n");
  printf("Return:   endless loop\n");
  return(ret);
 }
 //-----prepair exit main loop----------------------------------
 signal(SIGINT, my_signal_handler);    // <std c> handler
 g_pid_main=getpid();                  // pid of main program
 //------read config data---------------------------------------
 bRet=conf.readFile(_PFILE_CONF_);
 if(_DEBUG_ && bRet)  std::cout<< "Read config file: OK"<<std::endl;
 if(_DEBUG_ && !bRet) std::cout<< "Read config file: NOT OK"<<std::endl;
 if(_DEBUG_) conf.print();
 //------init network and mqtt----------------------------------
 try {
  g_myMosq.connect(phost, port, _KEEPALIVE_);
  g_myMosq.subscribe("#");
 } catch(std::string error) {
  fprintf(stderr,"Error while subscribing all topics: %s\n",error);
 }
 //------start separate periodic thread-------------------------
 std::thread mythread1(myMQTTperiodicExec, "");
 mythread1.detach();
 //------send start infos---------------------------------------
 if(conf.getStartTopic().length()>1) 
 {//.....MQTT: publish start message............................
  s1=conf.getStartPayload();
  if(conf.getAddTime()) s1+=" ("+getDateTime()+")";
  g_myMosq.publish(conf.getStartTopic(), s1,true);
 }
 g_smsBusy=false;                      // modem ready for sms
 if(conf.getStartSmsPhone().length()>2) 
 {//.....SMS: send start SMS....................................
  s1=conf.getStartSmsText();
  if(conf.getAddTime()) s1+=" ("+getDateTime()+")";
  std::thread threadSms(fSendSms, conf.getStartSmsPhone(), s1);
  threadSms.detach();
 }
 //------endless loop #1: wait for mqtt messages----------------
 // Edit an arrived message in MyMosquitto::onMessage
 g_endByXXX=4;
 g_myMosq.loop(true);
 terminate_program();
 return(0);
}
