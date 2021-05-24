//_____C_X232.h____________Karl Hartinger_____190501-190501_____
// Simple "c-like" class for serial interface RS232
// for linux and windows without exceptions, threads etc.
// #define LINUX_ 1 // for linux use, if not defined -> windows
// Hardware: (1) RS232 or USB-Serial-Adapter
// Released into the public domain.
// See also
// http://www.teuniz.net/RS-232/
// http://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html
// http://man7.org/linux/man-pages/man3/termios.3.html
// http://msdn.microsoft.com/en-us/library/windows/desktop/aa363145%28v=vs.85%29.aspx
// http://technet.microsoft.com/en-us/library/cc732236.aspx
#ifndef C_X232_H
#define C_X232_H

//#ifdef __cplusplus
//extern "C" {
//#endif
//#include "wb_common.h"                 // helper and object u

//.....important, if wb_common.h is not included................
#define LINUX_                  1     // 1=linux, 0=win
#include <string>                      // string
#include <stdio.h>                     // printf, sprintf
#include <stdlib.h>                    // exit
#include <string.h>                    // X232: strlen, strcpy
#include <stdarg.h>                    // X232: va_list,va_start
#include <dirent.h>                    // DIR, dirent, opendir
#include <sys/stat.h>                  // mkdir
#include <sys/types.h>                 // mkdir
#include <unistd.h>                    // stat

#define  String              std::string
#define  X232_BAUDRATE       9600
#define  X232_MODE           "8N1"
#define  X232_OK             0

// *************************************************************
//     switch between linux and windows compiling
// *************************************************************
#ifdef LINUX_
 #include <unistd.h>                   // usleep - LINUX, write
 #include <termios.h>                  // speed_t, B9600, ...
 #include <unistd.h>                   //
 #include <fcntl.h>                    //
 #include <sys/ioctl.h>                //
 #include <sys/types.h>                //
 #include <sys/stat.h>                 //
 #include <signal.h>                   // SIGINT
 #include <pthread.h>                  // pthread_create, ...
 #include <wiringPi.h>                 // pinMode, digitalRead
 #include <sys/file.h>                 // X232: flock

 #define  delay(ms) ( usleep(ms*1000L) )
 //-----C_X232--------------------------------------------------
 //#define  EOL                "\n"      // end of line
 #define  EOL                "\r\n"    // end of line
 //#define  X232_DEVICE        "/dev/ttyAMA0"
 //#define  X232_DEVICE        "/dev/ttyUSB0"
 #define  X232_DEVICE        "/dev/ttyS0"

 #define  X232_PORTMAX       38        //
/*
 #define  _PATH_LOGT_        "/var/www/html/waterbox/logT/"
 #define  _PATH_LOGM_        "/var/www/html/waterbox/logM/"
 #define  _PATH_LOOP_        "/var/www/html/waterbox/loop/"
 #define  _FILE_CONF_        "/var/www/html/waterbox/conf/wb.conf"
 #define  _FILE_MSGS_        "/var/www/html/waterbox/loop/wb.msg"
 #define  _FILE_NEW_SAMPLE_  "/var/www/html/waterbox/loop/newSample"
 #define  _FILE_NEW_DAY_     "/var/www/html/waterbox/loop/newDay"
 #define  _FILE_YESTERDAY_   "/var/www/html/waterbox/loop/yesterday"
*/
#else
 #include <windows.h>                  // Sleep(ms) - WINDOWS
 #include <conio.h>
 #define  delay(ms) ( Sleep(ms) )
 #define  EOL                "\r\n"    // end of line
 #define  X232_DEVICE        "COM1"
 //#define  X232_DEVICE        "COM3"
 #define  X232_PORTMAX       16        //
/*
 #define  _PATH_LOGT_        "./logT/"
 #define  _PATH_LOGM_        "./logM/"
 #define  _PATH_LOOP_        "./loop/"
 #define  _FILE_CONF_        "./wb.conf"
 #define  _FILE_MSGS_        "./loop/wb.msg"
 #define  _FILE_NEW_SAMPLE_  "./loop/newSample"
 #define  _FILE_NEW_DAY_     "./loop/newDay"
 #define  _FILE_YESTERDAY_   "./loop/yesterday"
*/
#endif
//^^^^^end of important, if wb_common.h is not included^^^^^^^^^


class X232 {
 protected:
 //-----properties----------------------------------------------
  String device_;
  long   baud_;
  String mode_;
  bool   nolfcr_;
#ifdef LINUX_
  int    fd;
  struct termios old_port_settings;
  struct termios new_port_settings;
#else
  HANDLE fd;
  char mode_str[128];
#endif // LINUX_

 public:
  //-----constructor & co---------------------------------------
  X232();
  X232(String device);
  X232(String device, long baudrate);
  X232(String device, long baudrate, String mode);
 protected:
  void   setup();                      // called by constructors
 public:
  void   initX232();                   // initialize properties
  //-----Setter and Getter methods------------------------------
  void setNolfcr(bool nolfcr) { nolfcr_=nolfcr; }
  //-----worker methods-----------------------------------------
  int  xopen();                        // tries 3x xopen1
  int  xopen1();                       // open method
  int  xclose();                       // close
  int  xputc(const char c);
  int  xputs(const char *buf, int size);
  int  xprint(const String s1);
  int  xprintln(String s1);
  int  xprintf(const char *message, ...);
  int  xdataAvail();
  int  xgetc();
  int  xgets(char *buf, int size);
  void xflush();
  void xflushRX();
  void xflushTX();
  int  xsetRTS(bool on_);
  int  xsetDTR(bool on_);
  int  xisDCD();
  int  xisCTS();
  int  xisDSR();
  int  xisRI();
};

//#ifdef __cplusplus
//}
#endif // C_X232_H
