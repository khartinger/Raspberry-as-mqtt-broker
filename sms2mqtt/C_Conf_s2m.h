//_____C_Conf_s2m.h_________Karl Hartinger_____210516-210516_____
// read data from sms2mqtt config file
#ifndef C_CONF_S2M_H
#define C_CONF_S2MH
#include <iostream>                    // cout
#include <string>                      // string
#include <map>                         // (multi)map
#include <vector>                      // vector
#include <fstream>                     // std::ifstream

#define _CONF_S2M_DEVICE_         "/dev/ttyS0"
#define _CONF_S2M_TOPICSUB_       "sms2mqtt/send"
#define _CONF_S2M_TOPICPUB_       "sms2mqtt/send/ret"
#define _CONF_S2M_ADD_TIME_       true
#define _CONF_S2M_START_TOPIC_    "info/start"
#define _CONF_S2M_START_PAYLOAD_  "sms2mqtt"
#define _CONF_S2M_END_TOPIC_      "info/end"
#define _CONF_S2M_END_PAYLOAD_    "sms2mqtt"
#define _CONF_S2M_START_SMS_NUM_  ""
#define _CONF_S2M_START_SMS_TXT_  ""
#define _CONF_S2M_END_SMS_NUM_    ""
#define _CONF_S2M_END_SMS_TXT_    ""
#define _CONF_S2M_CMD_END_        "cmd_end"
#define _CONF_S2M_CMD_VERSION_    "cmd_version"
#define _CONF_S2M_CMD_RELOAD_     "cmd_reload"

class Conf {
 protected:
 //-----properties----------------------------------------------
  bool dataOK;                         // true: file read, OK
  std::vector<std::string> vFrom;      // authorizised SMS from
  std::vector<std::string> vTo;        // authorizised SMS to
  std::string device;                  // serial interface
  std::string topicSub;                // topic base subscribe
  std::string topicPub;                // topic base publish
  bool addTime;                        // add date&time to msg
  std::string startTopic;              // topic info/start
  std::string startPayload;            // payload start prog
  std::string endTopic;                // topic info/end
  std::string endPayload;              // payload to end prog
  std::string startSmsPhone;           // On Start: Sms to number
  std::string startSmsText;            // On Start: Sms text
  std::string endSmsPhone;             // On End: Sms to number
  std::string endSmsText;              // On End: Sms text
  std::string cmdEnd;                  // command end of program
  std::string cmdVersion;              // command send version
  std::string cmdReload;               // cmd reload config file
 public:
  //-----constructor & co---------------------------------------
  Conf();
  void   initConf();                   // initialize properties
  //-----Setter and Getter methods------------------------------
  std::vector<std::string> getSmsFrom();
  std::vector<std::string> getSmsTo();
  std::string getDevice();
  std::string getTopicSub();
  std::string getTopicPub();
  bool        getAddTime();
  std::string getStartTopic();
  std::string getStartPayload();
  std::string getEndTopic();
  std::string getEndPayload();
  std::string getStartSmsPhone();
  std::string getStartSmsText();
  std::string getEndSmsPhone();
  std::string getEndSmsText();
  std::string getCmdEnd();
  std::string getCmdVersion();
  std::string getCmdReload();

  bool   isOK();
  //-----working methods----------------------------------------
  bool   readFile(std::string filename);
  bool   isAuthSmsFrom(std::string phoneNumber);
  bool   isAuthSmsTo(std::string phoneNumber);

  //-----helper methods-----------------------------------------
  void   delTrailLFCR(std::string& s1);
  void   delTrailBlank(std::string& s1);
  void   delLeadBlank(std::string& s1);
  void   delExtBlank(std::string &s1);
  void   replaceAll(std::string &str, const std::string& old_, const std::string& new_);
  void   strToLower(std::string &s1);
  void   splitString(std::string sIn, std::vector<std::string>&vOut, char delimiter);
  bool   split2String(std::string sIn,std::string& sPart1,std::string& sPart2,char delimiter);
  void   split2phone(std::string sIn, std::vector<std::string>&vOut);
  void   print();
};
#endif // C_CONF_S2M_H
