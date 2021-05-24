//_____C_Gsm.cpp___________Karl Hartinger_____190426-190728_____
#include "C_Gsm.h"

// *************************************************************
//    constructor & co
// *************************************************************
//_____constructor______________________________________________
Gsm::Gsm() {
 initGsm();
 setup();
}

Gsm::Gsm(String device_) {
 initGsm();
 device=device_;
 setup();
}

 Gsm::Gsm(String device_, long baudrate) {
 initGsm();
 device=device_;
 baud=baudrate;
 setup();
}

//_____init command properties__________________________________
void Gsm::initGsm()
{
 debug_gsm=DEBUG_GSM;                  // true: printf cmd...
 status=GSM_OK;                        // state of gsm module
 crlf_ =GSM_CRLF;                      // end of sms command
 mem_  =GSM_SMS_MEM;                   // "SM", "ME", "MT"
 numOfSms1_=GSM_SMS_NUM;               // number 1. sms location
 sCmd="";                              // GSM command
 sResult="";                           // answer of GSM modem
 sNetwork="";                          // registered network
 device=GSM_DEVICE;                    // ad serial interface
 //-----is there a device configuration file?-------------------
 std::string line1;
 std::ifstream fcnf(GSM_DEVICE_FILE);
 if(fcnf.good()) {
  std::getline(fcnf, line1);           // get line
  if(line1.length()>0) {               // length of line
   device=line1;
  }
 }
 else
 {
  if(debug_gsm) printf("Device file not found!\n");
 }
 baud=GSM_BAUDRATE;                    // ad serial interface
 initSms(sms1);                        //
 
}

//_____init sms property________________________________________
void Gsm::initSms(SSMS& sms0)
{
 sms0.number=-1;                       //
 sms0.phone="";                        //
 sms0.date="";                         //
 sms0.text="";                         //
}

//_____setup____________________________________________________
void Gsm::setup() {
 seri=X232(device, baud);
}

// *************************************************************
//    Setter and Getter methods
// *************************************************************

//_____name of modem device_____________________________________
String Gsm::getsDevice() { return device; }
  
//_____get status as int________________________________________
int Gsm::getStatus() { return status; }

//_____return status as string__________________________________
String Gsm::getsStatus() { return getsStatus(status); }

//_____return status as string__________________________________
String Gsm::getsStatus(int status1) {
 char s20[20];
 String s1;
 switch(status1) {
  case GSM_OK:       return "OK";
  case GSM_OK_END:   return "OK";
  case GSM_ERR_TOUT: return "ERROR: Send timeout";
  case GSM_NOMODULE: return "ERROR: No module found";
  case GSM_ERR_PROV: return "ERROR: No/wrong provider";
  case GSM_ERR_ATE0: return "ERROR while ATE0";
  case GSM_ERR_CMGF: return "ERROR while AT+CMGF";
  case GSM_ERR_CPMS: return "ERROR while AT+CPMS";
  case GSM_ERR_CNMI: return "ERROR while AT+CNMI";
  case GSM_ERR_NOGT: return "ERROR: No input sign >";
  case GSM_ERR_SEND: return "ERROR while sending sms";
  case GSM_ERR_CMGD: return "ERROR while deleting";
  case GSM_ERR_SNUM: return "ERROR: Wrong sms number";
  case GSM_ERR_NUM1: return "ERROR: No sms location 1";
  case GSM_ERR_NMEM: return "ERROR: Memory not set";
  case GSM_ERR_READ: return "ERROR: SMS not read";
  case GSM_ERR_NOSM: return "ERROR: No SMS";
  case GSM_ERR_CMGL: return "ERROR: SMS not listed";
  case GSM_ERR_COPS: return "ERROR: no network";
  case GSM_ERR_NOPH: return "ERROR: no phone number(s)";
  case GSM_ERR_OPEN: return "ERROR  while open serial";
  case GSM_ERR_TEXT: return "ERROR: Wrong text";
  case GSM_ERR_PARA: return "ERROR: Wrong parameter";
  case GSM_ERR_ANSW: return "ERROR: Send answer ERROR";
  case GSM_ERR_CCID: return "ERROR reading SIM card ID";
  case GSM_WORKING : return "**GSM module is working**";
  default: sprintf(s20,"%d",status1); s1=s20;
   return("ERROR: Unknown number "+s1);
 }
}

//_____get last command_________________________________________
String Gsm::getsCmd() {return sCmd; }

//_____result of last modem request_____________________________
String Gsm::getsResult() { return sResult; }

//_____result of last modem request_____________________________
// if replace_=true: replace \r by ^ and \n by ~
String Gsm::getsResult(bool raw_) {
 if(raw_) return sResult;
 String s1=sResult;
 s1=replacechar(s1, '\r', '^');
 s1=replacechar(s1, '\n', '~');
 return s1;
}

//_____get last retrieved sms1__________________________________
// for special sms: do readSms(num) first ;)
SSMS Gsm::getSms1() {return sms1; }

//_____return name of registered network________________________
String Gsm::getsNetwork() { return sNetwork; }

//_____get currently used memory ("SM", "ME" or "MT")___________
String Gsm::getsMem() { return mem_; }

//_____return memory number of command SMS______________________
// 1 for mem_="SM" or "MT", 21 or 31 if mem_="ME"
int Gsm::getNumOfSms1() { return numOfSms1_; }

//_____set memory for sms_______________________________________
// hardware access: set preferred memory for all actions
// return: status
int Gsm::setMem(String mem) {
 if((mem=="SM")||(mem=="ME")||(mem=="MT")) {
  mem_=mem;
  if(mem_=="ME") numOfSms1_=21; else numOfSms1_=1;
  sCmd="AT+CPMS=\""+mem_+"\",\""+mem_+"\",\""+mem_+"\"";
  if(!sendCmd(sCmd,4)) status=GSM_ERR_CPMS;
  else status=GSM_OK;
  return status;
 }
 status=GSM_ERR_NMEM;
 return status;
}

//_____set memory number of command SMS_________________________
int Gsm::setNumOfSms1(int num1) {
 if(num1>0) {
  numOfSms1_=num1;
  status=GSM_OK;
 }
 else status=GSM_ERR_SNUM;
 return status;
}

//_____set debug mode___________________________________________
void Gsm::setDebug(bool debug_) { debug_gsm=debug_; }

// *************************************************************
//    often used working methods
// *************************************************************

//_____Search for GSM module by sending AT command______________
// quick version
// return: true=OK, status GSM_OK
bool Gsm::isReady() {
 if(seri.xopen1()!=X232_OK) {          // only one trial
  sResult="**Serial ERROR**";          // class property
  status=GSM_ERR_OPEN;                 // error serial open
  return false;
 }
 seri.xclose();
 sCmd="AT";                            // test command
 if(!sendCmd(sCmd,2,GSM_CEND,200)) {   // send command
  status=GSM_NOMODULE;                 // set status to error
  return false;                        // no modem
 }
 status=GSM_OK;
 return true;
}

//_____Search for GSM module by sending AT command______________
// try to open RS232 3x
// return: true=OK, status GSM_OK
bool Gsm::isModule() {
 if(seri.xopen()!=X232_OK) {           // 3 trial
  delay(3000);
  if(seri.xopen()!=X232_OK) {          // 3 trial
   sResult="**Serial ERROR**";          // class property
   status=GSM_ERR_OPEN;                 // error serial open
   return false;
  }
 }
 seri.xclose();
 sCmd="AT";                            // test command
 if(!sendCmd(sCmd,2,GSM_CEND,200)) {   // send command
  status=GSM_NOMODULE;                 // set status to error
  return false;                        // no modem
 }
 status=GSM_OK;
 return true;
}

//_____begin = start sms module (setup module)__________________
// if a command fails, it will be repeated once
// return: true=OK, status set
bool Gsm::begin()
{
 //-----turn echo on (better for debug)-------------------------
 sCmd="ATE0";                          // echo 0=off, 1=on
 if(!sendCmd(sCmd,2,GSM_CEND,100)) {   // send command
  delay(80);                           // wait a little bit
  if(!sendCmd(sCmd,2,GSM_CEND,100)) {  // send command
   status=GSM_ERR_ATE0;                // set status to error
   return false;                       // return
  }
 }
 //-----set sms text format-------------------------------------
 sCmd="AT+CMGF=1";                     // use text format
 if(!sendCmd(sCmd)) {                  // 1. send command
  delay(80);                           // wait a little bit
  if(!sendCmd(sCmd)) {                 // 2. send command
   status=GSM_ERR_CMGF;                // set status to error
   return false;                       //
  }
 }
 //-----set preferred message storage to modem (ME)-------------
 setMem(GSM_SMS_MEM);                   // default ME
 if(status!=GSM_OK) {                   // if error...
  delay(80);                            // ...wait a little...
  setMem(GSM_SMS_MEM);                  // ...and try again.
 }
 if(status!=GSM_OK) return false;       // no success
 //-----calculate location of 1st SMS in memory-----------------
 readNumOfSms1();
 //-----get registered network AT+COPS?-------------------------
 sNetwork=readNetwork();               //read registered network
 if(status<=GSM_OK_END) return true;   // OK
 return false;                         //(this sets status ;)
}

//_____send a SMS_______________________________________________
// keep serial open for send phone number AND message!
// status: details about transmission
// return: true=GSM_OK or false=error
bool Gsm::sendSms(String phone, String text)
{
bool   bRet=false;                    // return value bool
 String sRet="";                       // return string
 char   ccmd[165];                     // sms text
 long   t;                             // time counter s
 int    ilen;                          // number of chars
 unsigned long ms;                     // time counter ms
 int    ic;                            // received char as int
 char   c;                             // help or received byte
 String sTemp;                         // help string
 String cmdEnd=GSM_ENDOFCMD;           // end of cmd as string
 //-----check phone number--------------------------------------
 ilen=phone.length();
 if(ilen<3 || ilen>100) { status=GSM_ERR_NOPH; return false; }
 c=phone.at(0);
 if(c!='+' && (c<'0' || c>'9')) { status=GSM_ERR_NOPH; return false; }
 if(text.length()>160) text=text.substr(0,160);
 //-----open serial---------------------------------------------
 if(debug_gsm) printf("Gsm::sendCmd ");
 if(seri.xopen()!=X232_OK) {           // 3x try to open, 100ms
  t=GSM_WAIT4SERI;
  while(t>0)
  {
   delay(900);
   if(seri.xopen()==X232_OK) break;
   t--;
  }
  if(t<=0)
  {
   sResult="**ERROR open serial**";     // class property
   status=GSM_ERR_OPEN;                 // error serial open
   if(debug_gsm) printf("%s return\n",sResult.c_str());
   return false;
  }
 }
 //-----send phone number to modem------------------------------
 sCmd="AT+CMGS=\""+phone+"\""+cmdEnd;
 //-----empty receive buffer, send sms command------------------
 seri.xflush();
 while(seri.xdataAvail()>0) {delay(2); if(seri.xgetc()<0) break;}
 delay(20);                            // wait a little bit
 strcpy(ccmd,sCmd.c_str());
 ilen=sCmd.length();
 for(int i=0; i<ilen; i++)             // send command...
  seri.xputc(ccmd[i]);                 // ..byte-wise
 //seri.xprint(cmd2);                  // send command string
 if(debug_gsm) print1ln(sCmd,"sent to gsm: ","\n");
 //-----wait for '>' char---------------------------------------
 ms=1000;                              // wait 1s for > sign
 sRet="";                              // return string
 delay(20+sCmd.length());              // wait a little bit
 for(t=ms/5; t>0; t--)
 {
  delay(5);
  if(seri.xdataAvail()>0)
  {
   ic=seri.xgetc();
   if(ic>=0)
   {
    c=(char)ic;
    //printf("|%c=%02x| ",c,ic);
    sRet=sRet+c;
    if(c=='>') break;
   }
  }
 }
 //-----ready for sending text?---------------------------------
 if(c!='>')                            // not ready
 {
  seri.xflush();
  seri.xclose();
  if(debug_gsm) printf("no text prompt\n");
  sResult=sRet;
  status=GSM_ERR_NOGT;
  return false;
 }
 //-----send sms text-------------------------------------------
 ilen=text.length();
 strcpy(ccmd,text.c_str());
 ccmd[ilen]=GSM_CENDOFSMS;
 ilen++;
 for(int i=0; i<ilen; i++)             // send sms text...
 { seri.xputc(ccmd[i]); delay(1); }    // ...byte-wise
 if(debug_gsm) print1ln(text,"sent to gsm: ","\n");
 //-----waiting for answer (minimum 5 sec!)---------------------
 ms=GSM_WAIT4SMS*1000;
 for(t=ms/10; t>0; t--)
 {
  delay(9);
  if(seri.xdataAvail()>0)
  {
   ic=seri.xgetc();
   if(ic>=0)
   {
    c=(char)ic;
//printf("|%c=%02x| ",c,ic);
    sRet=sRet+c;
    if(sRet.find("OK\r\n")!=String::npos) { t=0; bRet=true; }
    if(sRet.find("ERROR\r\n")!=String::npos) t=-2;
   }
  }
 } // t=t-1 (!)
 //-----end of sms-text?----------------------------------------
 if(t==0)
 {
  seri.xputc('\x1A');
  delay(100);
  if(debug_gsm) printf("<strg>Z\n");
 }
 seri.xflush();
 seri.xclose();
 //-----set status depending on t (end) value-------------------
 if(t==0)  status=GSM_ERR_TOUT;
 if(t==-2) status=GSM_OK_END;
 if(t<-2)  status=GSM_ERR_ANSW;
 if(debug_gsm) printf("Gsm::sendSmsText status %d, ",status);
 if(debug_gsm) print1ln(sRet, "received=|", "|\r\n");
 if(!debug_gsm) delay(100);
 sResult=sRet;
 return bRet;                          // return answer
}

//_____Is sms numOfSms1_ available?_____________________________
// return: true=yes, false=no
bool Gsm::isSms1() { return isSms(numOfSms1_); }

//_____read first SMS in used memory____________________________
// return: true=OK, false=no sms
bool Gsm::readSms1(SSMS& sms1) {
 sms1=readSms(numOfSms1_);
 if(status<=GSM_OK_END) return true;
 return false;
}

//_____delete first SMS in used memory__________________________
// return: true=yes, false=no
bool Gsm::deleteSms1() {
 bool ok=deleteSms(numOfSms1_);
 if(ok) initSms(sms1);
 return ok;
}

// *************************************************************
//     some more working methods
// *************************************************************

//_____Is sms available?________________________________________
// return: true=yes, false=no
bool Gsm::isSms(int num)
{
 char s20[20];                         // help char array
 String s1;                            // help string
 sprintf(s20,"%d",num);                // convert int to char[]
 s1=s20;                               // char[] to string
 sCmd="AT+CMGR="+s1;                   // read certain SMS
 if(sendCmd(sCmd,6,GSM_CEND,10000))
//printf("isSms() status=%d\r\n",status);
 if(status<=GSM_OK_END)
 {
  if(sResult.length()<40) return false;
  return true;
 }
 return false;
}

//_____read a certain SMS with given number_____________________
// ^~+CMGR: "REC READ","+43680xxx","","19/05/04,12:41:25+08"^~Test^~^~
// result in property sms1 and return sms
// return: struct SSMS
SSMS Gsm::readSms(int num)
{
 char s20[20];                         // help char array
 String s1;                            // help string
 sprintf(s20,"%d",num);                // convert int to char[]
 s1=s20;                               // char[] to string
 String phone_, date_, text_;          // received sms
 unsigned int iPos1, iPos2;            // index SMS string
 initSms(sms1);                        // clear sms1
 if(num<0) { status=GSM_ERR_SNUM; return sms1; }
 sCmd="AT+CMGR="+s1;                   // read certain SMS
 if(!sendCmd(sCmd, 5, GSM_CEND)) {
  delay(100);
  if(!sendCmd(sCmd, 5, GSM_CEND)) {
   status=GSM_ERR_READ; return sms1;
  }
 }
 //-----devide string to pieces---------------------------------
 if(sResult.find("CMGR:")==String::npos)
 { status=GSM_ERR_NOSM; return sms1; }
 //-----isolate phone number-------------------------------------
 iPos1=sResult.find(",\"");            // start phone number
 if(iPos1==String::npos) { status=GSM_ERR_READ; return sms1; }
 iPos1=iPos1+2;
 iPos2=sResult.find("\"", iPos1);     // end of phone number
 if(iPos2==String::npos) { status=GSM_ERR_READ; return sms1; }
 if(iPos2<=iPos1) { status=GSM_ERR_READ; return sms1; }
 phone_=sResult.substr(iPos1, iPos2-iPos1);
 //-----isolate date+time---------------------------------------
 iPos1=sResult.find(",\"", iPos2+2);   // start phone number
 if(iPos1==String::npos) { status=GSM_ERR_READ; return sms1; }
 iPos1=iPos1+2;
 iPos2=sResult.find("\"", iPos1+1);    // end of phone number
 if(iPos2==String::npos) { status=GSM_ERR_READ; return sms1; }
 if(iPos2<=iPos1) { status=GSM_ERR_READ; return sms1; }
 date_=sResult.substr(iPos1, iPos2-iPos1);
 //-----isolate sms text----------------------------------------
 iPos1=iPos2+3;
 iPos2=sResult.find("OK", iPos1+1);    // end of phone number
 if(iPos2==String::npos) { status=GSM_ERR_READ; return sms1; }
 iPos2=iPos2-1;
 if(iPos2<=iPos1) { status=GSM_ERR_READ; return sms1; }
 while(((sResult.at(iPos2)=='\r')||
         (sResult.at(iPos2)=='\n'))&&(iPos2>iPos1)) iPos2--;
 text_=sResult.substr(iPos1, iPos2-iPos1+1);
 sms1.number=num;
 sms1.phone=phone_;
 sms1.date=date_;
 sms1.text=text_;
 status=GSM_OK;
 return sms1;
}

//_____delete a certain SMS_____________________________________
// return: ERROR code
bool Gsm::deleteSms(int num)
{
 char s20[20];                         // help char array
 String s1;                            // help string
 sprintf(s20,"%d",num);                // convert int to char[]
 s1=s20;                               // char[] to string
 if(num<1) return GSM_ERR_SNUM;        //
 sCmd="AT+CMGD="+s1;                   // delete certain SMS
 if(!sendCmd(sCmd))                    // send command
 {//.....not OK: try again......................................
  delay(80);                           // wait a little bit
  if(!sendCmd(sCmd)) {                 // send command
   status=GSM_ERR_CMGD;                // set status to error
   return false;                       // sms not deleted
  }
 }
 status=GSM_OK;                        // set status to OK
 return true;                          // sms deleted
}

//_____list all SMS in used memory______________________________
String Gsm::listAllSms()
{
 int iNum=readNumSmsInMem();           // number of stored SMS
 unsigned long ms=GSM_WAIT_MS+500*iNum;//more time for answer
 sCmd="AT+CMGL=\"ALL\"";               // read all messages
 if(!sendCmd(sCmd,(3*iNum+2),GSM_CEND,ms)) // send cmd
 status=GSM_ERR_CMGL; else status=GSM_OK;
 return sResult;
}

//_____register to specified GSM network________________________
// net: "A1", "T-Mobile A", "one", ...
//  hardware access: register GSM network
bool Gsm::sendNetwork(String net) {
 sCmd="AT+COPS=1,0,\""+net+"\"";       // set Network
 if(!sendCmd(sCmd,4,GSM_CEND,20000)){
  delay(80);                           // wait a little bit
  if(!sendCmd(sCmd,4,GSM_CEND,20000)){
   status=GSM_ERR_COPS;                // set status to error
   return false;                       //
  }
 }
 delay(80);                            // wait a little bit
 sNetwork=readNetwork();               // check change
 readNumOfSms1();                      // may be changed!
 return true;                          //
}

//_____get registered network (provider) or empty string________
// hardware access: get network name from GSM module
String Gsm::readNetwork() {
 String s1="";                         // answer string
 unsigned int iPos1, iPos2;            // helper vars
 sCmd="AT+COPS?";                      // read
 if(!sendCmd(sCmd, 4))
  { status=GSM_ERR_COPS; return s1; }
 if(sResult.length()<15)
  { status=GSM_ERR_COPS; return s1; }
 //-----isolate provider name-----------------------------------
 iPos1=sResult.find(",\"");            // start of provider name
 if(iPos1==String::npos)               // not found
 { status=GSM_ERR_COPS; return s1; }
 iPos1=iPos1+2;
 iPos2=sResult.find("\"", iPos1);     // end of provider name
 if(iPos2==String::npos)               // not found
 { status=GSM_ERR_COPS; return s1; }
 if(iPos2<=iPos1)
 { status=GSM_ERR_COPS; return s1; }
 sNetwork=sResult.substr(iPos1, iPos2-iPos1);
 status=GSM_OK;
 return sNetwork;
}

//_____return number of stored SMS in selected mem______________
// hardware access: get number of stored SMS
// return: -1 on error
int Gsm::readNumSmsInMem() {
 unsigned int iPos1, iPos2;            // helper vars
 String s1;
 sCmd="AT+CPMS=\""+mem_+"\"";          // command
 {//.....not OK: try again......................................
  delay(80);                           // wait a little bit
  if(!sendCmd(sCmd,4)) {               // send command
   status=GSM_ERR_READ;                // set status to error
   return -1;                         // sms not deleted
  }
 }
 iPos1=sResult.find(": ");             // start
 if(iPos1==String::npos)               // not found
 { status=GSM_ERR_READ; return -1; }
 iPos1=iPos1+2;
 iPos2=sResult.find(",", iPos1);       // end
 if(iPos2==String::npos)               // not found
 { status=GSM_ERR_READ; return -1; }
 if(iPos2<=iPos1)
 { status=GSM_ERR_READ; return -1; }
 s1=sResult.substr(iPos1, iPos2-iPos1);
 //-----convert to int------------------------------------------
 return (int)atol(s1.c_str());
}

//_____return maximum number of SMS in selected mem_____________
// hardware access: get maximum number of SMS
// return: -1 on error
int Gsm::readMaxSmsInMem() {
 unsigned int iPos1, iPos2;            // helper vars
 String s1;
 sCmd="AT+CPMS=\""+mem_+"\"";          // command
 {//.....not OK: try again......................................
  delay(80);                           // wait a little bit
  if(!sendCmd(sCmd,4)) {               // send command
   status=GSM_ERR_READ;                // set status to error
   return -1;                         // sms not deleted
  }
 }
 iPos1=sResult.find(",");             // start
 if(iPos1==String::npos)               // not found
 { status=GSM_ERR_READ; return -1; }
 iPos1=iPos1+1;
 iPos2=sResult.find(",", iPos1);       // end
 if(iPos2==String::npos)               // not found
 { status=GSM_ERR_READ; return -1; }
 if(iPos2<=iPos1)
 { status=GSM_ERR_READ; return -1; }
 s1=sResult.substr(iPos1, iPos2-iPos1);
 //-----convert to int------------------------------------------
 return (int)atol(s1.c_str());
}

//_____read SIM integrated Circuit Card IDentification__________
// return ID or empty string
String Gsm::readSimId() {
 String s1;
 sCmd="AT+CCID";                       // command
 if(!sendCmd(sCmd,4)) {                // send cmd
  delay(80);                           // ...wait and try again
  if(!sendCmd(sCmd,4))
  { status=GSM_ERR_CCID; return ""; }
 }
 //-----remove OK-----------------------------------------------
 sResult=replaceString(sResult,"OK","");
 sResult=replaceString(sResult,"\r\n","");
 return sResult;
}

//_____request SMS with prepaid balance, return status__________
// method deletes first SMS location in actually used memory
// uses actual network provider
// return: true=request sent, false=error
bool Gsm::requestBalanceSMS(){return(requestBalanceSMS(sNetwork));}

//_____request SMS with prepaid balance, return status__________
// method deletes first SMS location in actually used memory
// uses given network provider
// return: true=request sent, false=error
bool Gsm::requestBalanceSMS(String sNet) {
 bool ret=false;
 unsigned long   t;                    // time counter
 String s1;
 //-----prepair memory location for SMS-------------------------
 sendAT();                             // for safety
 if(!deleteSms1()) return false;       // delete 1. sms location
 //-----do balance request for special provider-----------------
 if(sNet==GSM_NET_ID1) {
  //.....send request (voice call)..............................
  s1=GSM_NET_TEL1;
  sCmd="ATD"+s1+";";                   // voice call
  sendCmd(sCmd);                       // dial
  sCmd="AT+CLCC";                      // list current calls
  for(t=GSM_NET_MS_1/2000; t>0; t--)
  {
   delay(2000);
   sendCmd(sCmd);
   if(sResult.find("CLCC:")!=String::npos) t=0;
  }
  delay(50);                           // wait a little
  sCmd="ATH";                          // hang up...
  if(!sendCmd(sCmd)) {                 // ...anyway
   delay(50);                          // wait a little
   sendCmd(sCmd);                      // try again
  }
  ret=true;
 }
 //-----next provider-------------------------------------------
 else if(sNet==GSM_NET_ID2) {
  //.....send request (sms).....................................
  ret=sendSms(GSM_NET_TEL2, GSM_NET_TXT2); // request balance
 }
 //-----next provider-------------------------------------------
 else if(sNet==String(GSM_NET_ID3)) {
  //.....send request (sms).....................................
  ret=sendSms(GSM_NET_TEL3, GSM_NET_TXT3); // request balance
 }
 //-----unknown provider----------------------------------------
 else { };
 return ret;
}

//_____extract balance value from sms1__________________________
// return: Balance (credit) value as String or empty String
String Gsm::extractBalance() {
 return extractBalance(sms1.text);
}

//_____extract balance value from SMS___________________________
// return: Balance (credit) value as String or empty String
String Gsm::extractBalance(String smstext) {
 String sRet="";
 unsigned int iPos1, iPos2;            // helper vars
 //-----isolate balance value string from SMS textline----------
 if(sNetwork==GSM_NET_ID1) {
  iPos1=smstext.find("EUR ");
  if(iPos1==String::npos) return "";   // not found
  iPos1=iPos1+4;
  iPos2=smstext.find(" Gut",iPos1);    // end of balance value
  if(iPos2==String::npos) return "";   // not found
  if(iPos2<=iPos1) return "";          //
  sRet=smstext.substr(iPos1, iPos2-iPos1);
  sRet=replacechar(sRet, '.', ',');
 }
 //-----next provider-------------------------------------------
 else if(sNetwork==GSM_NET_ID2) {
  iPos1=smstext.find("gt ");
  if(iPos1==String::npos) return "";   // not found
  iPos1=iPos1+3;
  iPos2=smstext.find(" ",iPos1);       // end of balance value
  if(iPos2==String::npos) return "";   // not found
  if(iPos2<=iPos1) return "";          //
  sRet=smstext.substr(iPos1, iPos2-iPos1);
  sRet=replacechar(sRet, '.', ',');
 }
 //-----next provider-------------------------------------------
 else if(sNetwork==GSM_NET_ID3) {
  String s1=smstext;                   //
  s1=replaceString(s1,"00","");        //16 hex bit -> 8 bit hex
  s1=hexStringToString(s1);            // -> 8 bit ASCII
  iPos1=s1.find("gt ");
  if(iPos1==String::npos) return "";   // not found
  iPos1=iPos1+3;
  iPos2=s1.find(" ",iPos1+1);          // end of balance value
  if(iPos2==String::npos) return "";   // not found
  if(iPos2<=iPos1) return "";          //
  sRet=s1.substr(iPos1, iPos2-iPos1);
  sRet=replacechar(sRet, '.', ',');
 }
 //-----unknown provider----------------------------------------
 else { };
 return sRet;
}

// *************************************************************
//    helper methods
// *************************************************************

//_____Send AT command__________________________________________
// duration: about 100ms ... 200ms
// return: true = OK received, false ERROR
bool Gsm::sendAT() {
 sCmd="AT";                            // test command
 bool ret=true;                        // success
 if(!sendCmd(sCmd,2,GSM_CEND,GSM_WAIT_MS))  // send command
 {
  delay(100);
  ret=sendCmd(sCmd,2,GSM_CEND,GSM_WAIT_MS); // send command
 }
 return ret;
}

//_____send <strg>Z then AT command_____________________________
// clears serial buffer before sending AT command
// duration: about 100ms ... 2sec
// return: true = OK received, false ERROR
bool Gsm::sendZAT() {
 sCmd=GSM_CENDOFSMS;                   // <strg>z
 sendCmd(sCmd,4,GSM_CEND,GSM_WAIT_MS); // send command
 return sendAT();
}

//_____Send ATE0 or ATE1 command________________________________
// duration: about 100ms ... 200ms
// return: true = OK received, false ERROR
bool Gsm::sendATE0(bool E0=true) {
 if(E0) sCmd="ATE0";                   // test command no echo
 else sCmd="ATE1";                     // yes echo
 bool ret=true;                        // success
 if(!sendCmd(sCmd,2,GSM_CEND,100))     // send command
 {
  delay(100);
  ret=sendCmd(sCmd,2,GSM_CEND,100);    // send command
 }
 return ret;
}


// *************************************************************
//      internal functions
// *************************************************************

//_____send a command to SIM800C with default values____________
// numEnd=2   : how often should end char be found in answer
// cEnd  ='\n': last char of answer
// ms    =6000: timeout in milliseconds
// return: true=cmd sent with answer OK, or false + sResult set
bool Gsm::sendCmd(String cmd) {
 return(sendCmd(cmd, GSM_NUM_END, GSM_CEND, GSM_WAIT_MS));
}

//_____send a command to SIM800C________________________________
bool Gsm::sendCmd(String cmd, int numEnd) {
 return(sendCmd(cmd, numEnd, GSM_CEND, GSM_WAIT_MS));
}

//_____send a command to SIM800C________________________________
bool Gsm::sendCmd(String cmd, int numEnd, char cEnd) {
 return(sendCmd(cmd, numEnd, cEnd, GSM_WAIT_MS));
}

//_____send a command to SIM800C________________________________
// numEnd: how often should end char be found in answer
// cEnd  : last char of answer
// ms    : timeout in milliseconds
// minimum duration: about 100ms
// return: true=cmd sent with answer OK, or false + sResult set
bool Gsm::sendCmd(String cmd, int numEnd, char cEnd, unsigned long ms)
{
 char   ccmd[162];
 String sRet="";                       // answer string
 bool   ret=false;                     // cmd no success
 int    ic;                            // received char as int
 char   c;                             // received byte
 String cmd2=cmd;                      // command + carriage ret
 String cmdEnd=GSM_ENDOFCMD;           // end of cmd as string
 String smsEnd=GSM_ENDOFSMS;           // end of sms as string
 int    cntEnd=0;                      // number of end chars
 long   t;                             // time counter
 sCmd=cmd;                             // command to do
 if(debug_gsm) printf("Gsm::sendCmd ");
 if(seri.xopen()!=X232_OK) {
  sResult="**Serial ERROR**";          // class property
  status=GSM_ERR_OPEN;                 // error serial open
  if(debug_gsm) printf("%s return\n",sResult.c_str());
  return ret;
 }
 //-----empty receive buffer, send command----------------------
 seri.xflush();
 while(seri.xdataAvail()>0) {delay(2); if(seri.xgetc()<0) break;}
 delay(20);                            // wait a little bit
 if(cmd2.length()>159) cmd2=cmd2.substr(0,159);
 if(cmd2.find(smsEnd)==String::npos)   // end of sms text?
 {
  if(cmd2.find(cmdEnd)==String::npos)  // end of cmd?
   cmd2=cmd2+cmdEnd;                   // no: add end of cmd
 }
 int len=cmd2.length();
 strcpy(ccmd,cmd2.c_str());
 for(int i=0; i<len; i++) seri.xputc(ccmd[i]);

 //seri.xprint(cmd2);                    // send command
 if(debug_gsm) print1ln(cmd2,"sent to gsm: ","\n");
 //-----receive answer------------------------------------------
 delay(20+cmd2.length());              // wait a little bit
 for(t=ms/5; t>0; t--)
 {
  delay(5);
  if(seri.xdataAvail()>0)
  {
   ic=seri.xgetc();
   if(ic>=0)
   {
    c=(char)ic;
//printf("|%c=%02x| ",c,ic);
    sRet=sRet+c;
    if(c==cEnd) cntEnd++;              // end char detected
    if(cntEnd>=numEnd) t=-1;            // enough end chars
    if(sRet.find("OK\r\n")!=String::npos) { t=0; ret=true; }
    if(sRet.find("ERROR\r\n")!=String::npos) t=-2;
   }
  }
 }
 seri.xflush();
 //-----end of sms-text?----------------------------------------
 if(t==0)
 {
  if(cmd2.find(smsEnd)!=String::npos) {
    seri.xputc('\x1A');
    if(debug_gsm) printf("<strg>Z\n");
  }
 }
 seri.xclose();
 if(t==0)  status=GSM_ERR_TOUT;
 if(t==-1) status=GSM_OK;
 if(t==-2) status=GSM_OK_END;
 if(t<-2)  status=GSM_ERR_ANSW;
 if(debug_gsm) printf("Gsm::sendCmd status %d, ",status);
 if(debug_gsm) print1ln(sRet, "received=|", "|\r\n");
 if(!debug_gsm) delay(100);
 sResult=sRet;
 return ret;                          // return answer string
}

//_____return memory number of command SMS______________________
// hardware access: get max. number of SMS in SIM memory SM
// +CPMS: 0,30,0,50,0,50\r\n\r\n
// 1 for mem_="SM" or "MT", 21 or 31 if mem_="ME" (-1 ERROR)
int Gsm::readNumOfSms1() {
 unsigned int iPos1, iPos2;
 if(mem_!="ME") numOfSms1_=1;
 else {
  numOfSms1_=21;                       // default value
  String sC="AT+CPMS=\"SM\"";          // command change mem
  if(!sendCmd(sC,4)) return -1;        // set mem to SM
  String s1=sResult;                   // last result
  setMem(mem_);                        // set mem back
  //-----analyze first answer string s1-------------------------
  iPos1=s1.find(",");
  if(iPos1==String::npos)return numOfSms1_; //error->default val
  iPos1++;
  if(iPos1<2) return numOfSms1_;       // error -> default value
  iPos2=s1.find(",", iPos1);
  if(iPos2==String::npos)return numOfSms1_; //error->default val
  if(iPos2<=iPos1) return numOfSms1_;  //error->default val
  s1=s1.substr(iPos1, iPos2-iPos1);
  int i=atoi(s1.c_str());
  if(i>0) numOfSms1_=1+i;
 }
 return numOfSms1_;
}

//_____print string as 1 line: Replace \r with ^, \n with ~_____
void Gsm::print1ln(String s1) {
 print1ln(s1,"", "\r\n");
}

//_____print string as 1 line: Replace \r with ^, \n with ~_____
void Gsm::print1ln(String s1, String before_) {
 print1ln(s1, before_, "\r\n");
}

//_____print string as 1 line: Replace \r with ^, \n with ~_____
void Gsm::print1ln(String s1, String before_, String after_)
{
 String s2=s1;
 printf("%s",before_.c_str());
 for(size_t i=0; i<s2.length(); i++)
 {
  if(s2[i]=='\r') s2[i]='^';
  if(s2[i]=='\n') s2[i]='~';
  if(s2[i]=='\x1A') s2[i]='$';
 }
 printf("%s%s",s2.c_str(),after_.c_str());
}

//__print result string as 1 line: Replace \r with ^, \n with ~_
void Gsm::print1lnResult() { print1ln(sResult, "", "\r\n"); }

//__print result string as 1 line: Replace \r with ^, \n with ~_
void Gsm::print1lnResult(String before_) {
 print1ln(sResult, before_, "\r\n");
}

//__print result string as 1 line: Replace \r with ^, \n with ~_
void Gsm::print1lnResult(String before_, String after_) {
 print1ln(sResult, before_, after_);
}

//_____print sms1_______________________________________________
void Gsm::printSms1() { printSms(sms1); }

//_____print sms________________________________________________
void Gsm::printSms(struct SSMS sms0)
{
 printf("Num   |%d|\r\n",sms0.number);
 printf("Phone |%s|\r\n",sms0.phone.c_str());
 printf("Date  |%s|\r\n",sms0.date.c_str());
 printf("Text  |%s|\r\n",sms0.text.c_str());
}

//_____replace all matching characters in a string to another___
String Gsm::replacechar(String s1, char cold, char cnew) {
 String s2=s1;
 for(size_t i=0; i<s2.length(); i++)
 {
  if(s2[i]==cold) s2[i]=cnew;
 }
 return s2;
}

//_____replace part of string to another________________________
String Gsm::replaceString(String s1, const String sold, const String snew) {
 if(sold.empty()) return s1;           // nothing to replace
 size_t iPos1=s1.find(sold);           // first position to replace
 if(iPos1==String::npos) return s1;    // nothing to replace
 size_t iPos2=iPos1+sold.length();
 return s1.substr(0,iPos1)+snew+replaceString(s1.substr(iPos2,s1.length()-iPos2),sold,snew);
}

//_____convert HEX-String to ASCII-String_______________________
// e.g. 48414C4C4F -> HALLO
String Gsm::hexStringToString(String sHex)
{
 String sRet="";
 String s1;
 int len_=sHex.length();
 for(int i=0; i<len_-1; i=i+2)
 {
  char cc=(hex2int(sHex.at(i))<<4)|hex2int(sHex.at(i+1));
  s1=cc;
  sRet+=s1;
 }
 return sRet;
}
