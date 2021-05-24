//_____C_Gsm.h______________Karl Hartinger_____190426-190728_____
#ifndef C_GSM_H
#define C_GSM_H
#include "C_X232.h"
#include <fstream>                     // std::ifstream

//.....important, if wb_common.h is not included................
#define  DEBUG_GSM           false
#define  String              std::string

#define  GSM_DEVICE          "/dev/ttyS0"
//#define  GSM_DEVICE          "/dev/ttyUSB0"
#define  GSM_DEVICE_FILE     "../device.conf"
#define  GSM_BAUDRATE        9600
#define  GSM_CRLF            "\r"      // sim command: end chars
#define  GSM_NUM_END         2         // number of cEnd in answer
#define  GSM_WAIT_MS         6000      // timeout gsm command 6s
#define  GSM_WAIT4SERI       10        // wait 10s serial free
#define  GSM_WAIT4SMS        7         // wait 7s for sms answer
#define  GSM_CEND            '\n'      // sim answer: end char
#define  GSM_CENDOFSMS       '\x1A'    // end of sms text
#define  GSM_ENDOFSMS        "\x1A"    // end of sms text
#define  GSM_ENDOFCMD        "\r"      // sim command: end chars
#define  GSM_SMS_MEM         "ME"      // ME=modem, SM=sim card
#define  GSM_SMS_NUM         31        // sms# ME: 21, SM:1
//-------provider/network data (Austria)------------------------
#define  GSM_NET_ID1         "A1"           //networkID B.free
#define  GSM_NET_TEL1        "08006642903"  //b.free phone numb
#define  GSM_NET_MS_1        29000          //A1 wait max.29sec
#define  GSM_NET_ID2         "T-Mobile A"   //networkID HOT
#define  GSM_NET_TEL2        "6700"         //Hot phone number
#define  GSM_NET_TXT2        "GUTHABEN"     //Hot balance text
#define  GSM_NET_ID3         "one"          //networkID Drei
#define  GSM_NET_TEL3        "31321"        //Drei phone number
#define  GSM_NET_TXT3        "GUT"          //Drei balance text
//-------status-------------------------------------------------
#define  GSM_OK              0         // success
#define  GSM_OK_END          1         // success
#define  GSM_ERR_TOUT        2         // ERROR send timeout
#define  GSM_NOMODULE        3         // ERROR no module found
#define  GSM_ERR_PROV        4         // ERROR no/wrong provider
#define  GSM_ERR_ATE0        5         // ERROR while ATE0
#define  GSM_ERR_CMGF        6         // ERROR while AT+CMGF
#define  GSM_ERR_CPMS        7         // ERROR while AT+CPMS
#define  GSM_ERR_CNMI        8         // ERROR while AT+CNMI
#define  GSM_ERR_NOGT        9         // ERROR no input sign >
#define  GSM_ERR_SEND        10        // ERROR while sending sms
#define  GSM_ERR_CMGD        11        // ERROR while deleting
#define  GSM_ERR_SNUM        12        // ERROR wrong sms number
#define  GSM_ERR_NUM1        13        // ERROR no sms location 1
#define  GSM_ERR_NMEM        14        // ERROR memory not set
#define  GSM_ERR_READ        15        // ERROR SMS not read
#define  GSM_ERR_NOSM        16        // ERROR No SMS
#define  GSM_ERR_CMGL        17        // ERROR SMS not listed
#define  GSM_ERR_COPS        18        // ERROR no network
#define  GSM_ERR_NOPH        19        // ERROR no phone number(s)
#define  GSM_ERR_OPEN        20        // ERROR while open serial
#define  GSM_ERR_TEXT        21        // ERROR wrong text
#define  GSM_ERR_PARA        22        // ERROR wrong parameter
#define  GSM_ERR_ANSW        23        // ERROR send answer ERROR
#define  GSM_ERR_CCID        24        // ERROR reading SIM card ID
#define  GSM_WORKING         -2        // Sending SMS

//_____convert hex char to value (e.g. C -> 12)_________________
#define hex2int(c)( (c>='0' && c<='9') ? (c-'0') : (9+(c&0xF)) )
//^^^^^end of important, if wb_common.h is not included^^^^^^^^^

//_____info about a SMS_________________________________________
struct SSMS {
 int  number;
 String phone;
 String date;
 String text;
};

class Gsm {
 //-----properties----------------------------------------------
 protected:
  int    status;                       // state of gsm module
  String crlf_;                        // end of sms command
  String mem_;                         // "SM", "ME", "MT"
  int    numOfSms1_;                   // number 1. sms location
  String sCmd;                         // GSM command
  String sResult;                      // answer of GSM modem
  String sNetwork;                     // registered network
  String device;                       // serial device
  long   baud;                         // serial baudrate
  X232   seri;                         // serial interface
  SSMS   sms1;                         // last received sms
  bool   debug_gsm;
 public:
  //-----constructor & co---------------------------------------
  Gsm();
  Gsm(String device_);
  Gsm(String device_, long baudrate);
 protected:
  void   setup();                      // called by constructors
 public:
  void   initGsm();                    // initialize properties
  void   initSms(SSMS& sms0);          // initialize struct SSMS
  //-----Setter and Getter methods------------------------------
  String getsDevice();                 // name of modem device
  int    getStatus();                  // state # of last cmd
  String getsStatus();                 // state as string
  String getsStatus(int status1);      // state as string
  String getsCmd();                    // get last command
  String getsResult();                 // last modem answer
  String getsResult(bool raw);         // last modem answer
  SSMS   getSms1();                    // last retrieved sms1
  String getsNetwork();                // registered network
  String getsMem();                    // "SM", "ME", "MT"
  int    getNumOfSms1();               // mem number 1 of sms
  int    setMem(String mem);           // set memory for sms
  int    setNumOfSms1(int num1);       // mem number 1 of sms
  void   setDebug(bool debug_);        // true = debug
  //-----often used working methods-----------------------------
  bool   isReady();                    // module ready?
  bool   isModule();                   // module answering AT?
  bool   begin();                      // start module
  bool   sendSms(String phone, String text);
  bool   isSms1();                     //sms numOfSms1_ available?
  bool   readSms1(SSMS& sms1);         // read sms1 from memory
  bool   deleteSms1();                 // delete first SMS
  //------some more working methods-----------------------------
  bool   isSms(int num);               // is sms num available?
  SSMS   readSms(int num);             // read sms num
  bool   deleteSms(int num);           // delete SMS number num
  String listAllSms();                 // list all sms as string
  bool   sendNetwork(String net);      // set gsm network
  String readNetwork();                // registered gsm network
  int    readNumSmsInMem();            // number of stored SMS
  int    readMaxSmsInMem();            // max. number of SMS
  String readSimId();                  // SIM Card ID
  bool   requestBalanceSMS();          // for provider sNetwork
  bool   requestBalanceSMS(String sNet); // for given provider
  String extractBalance();             // provider specific
  String extractBalance(String smstext);// provider specific
  //-----helper methods-----------------------------------------
  bool   sendAT();                     // send AT
  bool   sendZAT();                    // send <strg>z, then AT
  bool   sendATE0(bool E0);            // send ATE0 or ATE1
  //-----internal methods---------------------------------------
  bool   sendCmd(String cmd);
  bool   sendCmd(String cmd, int numEnd);
  bool   sendCmd(String cmd, int numEnd, char cEnd);
  bool   sendCmd(String cmd, int numEnd, char cEnd, unsigned long ms);
  int    readNumOfSms1();              // calc num using mem
  void   print1ln(String s1);
  void   print1ln(String s1, String before_);
  void   print1ln(String s1, String before_, String after_);
  void   print1lnResult();
  void   print1lnResult(String before_);
  void   print1lnResult(String before_, String after_);
  void   printSms1();
  void   printSms(struct SSMS sms0);
  String replacechar(String s1, char cold, char cnew);
  String replaceString(String s1, const String sold, const String snew);
  String hexStringToString(String sHex); // HEX- to ASCII-string
  //bool   sendSmsText(String cmd);
};

#endif // C_GSM_H
