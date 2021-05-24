//_____C_Conf_s2m.cpp_______Karl Hartinger_____210523-210523_____
// 1. read data from configuration file "sms2mqtt.conf"
// 2. contain all config data for program sms2mqtt
#include "C_Conf_s2m.h"

// *************************************************************
//    constructor & co
// *************************************************************
//_____constructor______________________________________________
Conf::Conf() {
 initConf();
}

//_______init config properties_________________________________
void Conf::initConf(){
 dataOK=false;                         // config data ok
 vFrom.clear();                        // authorizised SMS from
 vTo.clear();                          // authorizised SMS to
 device       =_CONF_S2M_DEVICE_;
 topicSub     =_CONF_S2M_TOPICSUB_;
 topicPub     =_CONF_S2M_TOPICPUB_;
 addTime      = _CONF_S2M_ADD_TIME_;
 startTopic   =_CONF_S2M_START_TOPIC_;
 startPayload =_CONF_S2M_START_PAYLOAD_;
 endTopic     =_CONF_S2M_END_TOPIC_;
 endPayload   =_CONF_S2M_END_PAYLOAD_;
 startSmsPhone=_CONF_S2M_START_SMS_NUM_;
 startSmsText =_CONF_S2M_START_SMS_TXT_;
 endSmsPhone  =_CONF_S2M_END_SMS_NUM_;
 endSmsText   =_CONF_S2M_END_SMS_TXT_;
 cmdEnd       =_CONF_S2M_CMD_END_;
 cmdVersion   =_CONF_S2M_CMD_VERSION_;
 cmdReload    =_CONF_S2M_CMD_RELOAD_;
}

// *************************************************************
//    Setter and Getter methods
// *************************************************************

std::vector<std::string> Conf::getSmsFrom() { return vFrom; }
std::vector<std::string> Conf::getSmsTo() { return vTo; }
std::string Conf::getDevice()        { return device; }
std::string Conf::getTopicSub()      { return topicSub; }
std::string Conf::getTopicPub()      { return topicPub; }
bool        Conf::getAddTime()       { return addTime; }
std::string Conf::getStartTopic()    { return startTopic; }
std::string Conf::getStartPayload()  { return startPayload; }
std::string Conf::getEndTopic()      { return endTopic; }
std::string Conf::getEndPayload()    { return endPayload; }
std::string Conf::getStartSmsPhone() { return startSmsPhone; }
std::string Conf::getStartSmsText()  { return startSmsText; }
std::string Conf::getEndSmsPhone()   { return endSmsPhone; }
std::string Conf::getEndSmsText()    { return endSmsText; }
std::string Conf::getCmdEnd()        { return cmdEnd; }
std::string Conf::getCmdVersion()    { return cmdVersion; }
std::string Conf::getCmdReload()     { return cmdReload; }

bool Conf::isOK() { return dataOK; }

// *************************************************************
//    working methods
// *************************************************************

//_______read config file_______________________________________
// return true: read data ok
bool Conf::readFile(std::string pathfilename)
{
 //-----temporary data for reading file------------------------
 int iConf=13;
 std::string tempKey[iConf]={"from","to","device","sub","pub",
  "addtime","startmqtt","endmqtt","startsms","endsms",
  "cmdend", "cmdversion", "cmdreload"};
 std::string tempVal[iConf]={"?","?","?","?","?","?","?","?","?","?","?","?","?"};
 std::string line_;
 std::ifstream fcnf(pathfilename.c_str());
 std::string key1, val1;
 if(!fcnf.good()) { dataOK=false; return dataOK; }
 //======read data from file====================================
 while(fcnf.good())
 {//----------read lines until new section found or last line---
  std::getline(fcnf, line_);           // get line
  delTrailLFCR(line_);                 // delete \r \n 
  delExtBlank(line_);                  // delete blanks
  //std::cout << "Line >" << line_ << "<" << std::endl;
  if(line_.length()>0)                 // length of line
  {//---------not an empty line---------------------------------
   if(line_.at(0)!='#')                // comment line?
   {//--------not a comment line--------------------------------
    if(split2String(line_,key1,val1,':'))
    {//......valid pair........................................
     strToLower(key1);
     for(int i=0; i<iConf; i++)
     {
      if(key1==tempKey[i]) { tempVal[i]=val1; break; }
     }
    } // end valid pair
   } // end comment line
  } // end empty line
 }
 fcnf.close();
 //======check read data {from,to,device,sub,pub,infostart}=====
 //------0: check all phone numbers for "sms from"--------------
 std::vector<std::string> tempvFrom;
 if(tempVal[0]=="?") { dataOK=false; return dataOK; }
 split2phone(tempVal[0],tempvFrom);
 if(tempvFrom.size()<1) { dataOK=false; return dataOK; }
 //------1: check all phone numbers for "sms to"----------------
 std::vector<std::string> tempvTo;
 if(tempVal[1]=="?") { dataOK=false; return dataOK; }
 split2phone(tempVal[1],tempvTo);
 if(tempvTo.size()<1) { dataOK=false; return dataOK; }
 //------copy temp values to properties-------------------------
 vFrom=tempvFrom;                           // 0=from
 vTo=tempvTo;                               // 1=to
 if(tempVal[2]!="?") device=tempVal[2];     // 2=device
 if(tempVal[3]!="?") topicSub=tempVal[3];   // 3=sub
 if(tempVal[4]!="?") topicPub=tempVal[4];   // 4=pub
 if(tempVal[5]!="?") {                      // 5=endmqtt
  if(tempVal[5]=="no"||tempVal[5]=="nein")  addTime=false; 
   else addTime=true;
 }
 if(tempVal[6]!="?") {                      // 6=startmqtt
  if(tempVal[6]=="") { startTopic=""; startPayload=""; }
  else split2String(tempVal[6],startTopic,startPayload,' ');
 }
 if(tempVal[7]!="?") {                      // 7=endmqtt
  if(tempVal[7]=="") { endTopic=""; endPayload=""; }
  else split2String(tempVal[7],endTopic,endPayload,' ');
 }
 if(tempVal[8]!="?") {
  if(tempVal[8]=="") { startSmsPhone=""; startSmsText=""; }
  else split2String(tempVal[8],startSmsPhone,startSmsText,' ');
 }
 if(tempVal[9]!="?") { 
  if(tempVal[9]=="") { endSmsPhone=""; endSmsText=""; }
  else split2String(tempVal[9],endSmsPhone,endSmsText,' ');
 }
 if(tempVal[10]!="?") cmdEnd=tempVal[10];    // 10=command end
 if(tempVal[11]!="?") cmdVersion=tempVal[11];// 11=command version
 if(tempVal[12]!="?") cmdReload=tempVal[12]; // 12=command reload
 dataOK=true;
 return dataOK;
}

//_______is given phone number included in "from list"?_________
bool Conf::isAuthSmsFrom(std::string phoneNumber)
{
 for(int i=0; i<vFrom.size(); i++) {
  if(vFrom.at(i)==phoneNumber) return true;
 }
 return false;
}

//_______is given phone number included in "to list"?___________
bool Conf::isAuthSmsTo(std::string phoneNumber)
{
 for(int i=0; i<vTo.size(); i++) {
  if(vTo.at(i)==phoneNumber) return true;
 }
 return false;
}

// *************************************************************
//    helper methods
// *************************************************************

//_____remove line feed and carriage return from end of line____
void Conf::delTrailLFCR(std::string& s1) {
 bool goon=true;
 int i=s1.length()-1;
 if(i<0) return;
 while((i>=0)&&goon)
 {
  if((s1.at(i)=='\r')||(s1.at(i)=='\n')) i--;
  else goon=false;
 }
 s1.erase(i+1, std::string::npos);
}


//_____remove blank(s) from end of line_________________________
void Conf::delTrailBlank(std::string& s1) {
 bool goon=true;
 int i=s1.length()-1;
 if(i<0) return;
 while((i>=0)&&goon)
 {
  if(s1.at(i)==' ') i--;
  else goon=false;
 }
 s1.erase(i+1, std::string::npos);
}

//_____remove blank(s) from begin of line_______________________
void Conf::delLeadBlank(std::string& s1) {
 int len=s1.length();
 if(len<1) return;
 int i=0;
 while(i<len) {
  if(s1.at(i)==' ') i++; else break;
 }
 if(i>0) s1.erase(0,i);
}

//_____remove blank(s) from begin and end of line_______________
void Conf::delExtBlank(std::string &s1) {
 delTrailBlank(s1);
 delLeadBlank(s1);
}

//_____replace a part of the string to another__________________
void Conf::replaceAll(std::string &str, const std::string& old_, const std::string& new_)
{
 size_t start_pos = 0;
 while((start_pos = str.find(old_, start_pos)) != std::string::npos) {
  str.replace(start_pos, old_.length(), new_);
  start_pos += new_.length();
 }
}

//_____convert string to lower string___________________________
void Conf::strToLower(std::string &s1)
{
 std::string s2="";
 int    ilen=s1.length();
 for(int i=0; i<ilen; i++)
 {
  char c=s1.at(i);
  c=tolower(c);
  s2=s2+c;
 }
 s1=s2;
}

//_______split string to vector_________________________________
void Conf::splitString(std::string sIn, std::vector<std::string>&vOut, char delimiter)
{
 std::string temp="";
 for(int i=0; i<sIn.size(); i++)
 {
  if(sIn[i] == delimiter)
  {
   vOut.push_back(temp);
   temp="";
  }
  else
   temp += sIn[i];
 }
 vOut.push_back(temp);
}

//_______split string to 2 strings______________________________
// return: true=split ok, sPart1 and sPart2 changed
//         false=delimiter not found or one sPart is empty
//               sPart1 and sPart2 NOT changed!
bool Conf::split2String(std::string sIn,std::string &sPart1,
  std::string &sPart2,char delimiter)
{
 delExtBlank(sIn);                      // delete blanks begin/end
 int idp=sIn.find_first_of(delimiter);  // search for...
 if(idp!=std::string::npos)             // ...delimiter
 {//-------delimiter found -> line is a pair <key value>--------
  std::string key1=sIn.substr(0,idp);
  std::string val1=sIn.substr(idp+1);
  delExtBlank(key1);                   // delete blanks begin/end
  delExtBlank(val1);                   // delete blanks begin/end
  //if(key1.length()>0 && val1.length()>0) 
  if(key1.length()>0)
  {//--------split OK-------------------------------------------
   sPart1=key1;                        // part 1
   sPart2=val1;                        // part 2
   return true;
  } // end split OK
 } // end delimiter found
 return false;
}

//_______split string to phone numbers__________________________
// aaa,bbb,ccc -> vector
void Conf::split2phone(std::string sIn, std::vector<std::string>&vOut)
{
 char delimiter=',';
 char c1;
 bool bPhoneNum=true;
 std::string temp="";
 for(int i=0; i<sIn.size(); i++)
 {//----------for each char-------------------------------------
  c1=sIn.at(i);
  if(c1!=' ')
  {//---------char is not blank---------------------------------
   if(sIn[i] == delimiter)
   {//--------char is delimiter---------------------------------
    if(bPhoneNum) {
     if(temp.length()>2) vOut.push_back(temp);
    }
    temp="";
    bPhoneNum=true;
   }// end char is delimiter
   else
   {//--------char is not the delimiter-------------------------
    if((c1>='0' && c1<='9') || (temp=="" && c1=='+'))
     temp+=c1;
    else
     bPhoneNum=false;
   } // end char is not the delimiter
  } // end char is not blank
 } // end for each char
 if(bPhoneNum) { // last phone number
  if(temp.length()>2) vOut.push_back(temp);
 }
}

//_______print properties_______________________________________
void Conf::print()
{
 std::cout<< "SMS from: ";
 for(int i=0; i<getSmsFrom().size(); i++)
  std::cout<< getSmsFrom().at(i)<<" | ";
 std::cout<<std::endl;
 std::cout<< "SMS to  : ";
 for(int i=0; i<getSmsTo().size(); i++)
  std::cout<< getSmsTo().at(i)<<" | ";
 std::cout<<std::endl;
 std::cout<< "device      : " << getDevice() <<std::endl;
 std::cout<< "topicSub    : " << getTopicSub() <<std::endl;
 std::cout<< "topicPub    : " << getTopicPub() <<std::endl;
 std::cout<< "startTopic  : " << getStartTopic() <<std::endl;
 std::cout<< "startPayload: " << getStartPayload() <<std::endl;
 std::cout<< "endTopic    : " << getEndTopic() <<std::endl;
 std::cout<< "endPayload  : " << getEndPayload() <<std::endl;
 std::cout<< "startSmsPhone: " << getStartSmsPhone() <<std::endl;
 std::cout<< "startSmsText: " << getStartSmsText() <<std::endl;
 std::cout<< "endSmsPhone : " << getEndSmsPhone() <<std::endl;
 std::cout<< "endSmsText  : " << getEndSmsText() <<std::endl;
 std::cout<< "cmdEnd      : " << getCmdEnd() <<std::endl;
 std::cout<< "cmdVersion  : " << getCmdVersion() <<std::endl;
 std::cout<< "cmdReload   : " << getCmdReload() <<std::endl;
}
