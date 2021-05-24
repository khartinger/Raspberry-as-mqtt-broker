//_____C_X232.cpp_________Karl Hartinger______190501-190501_____
// Simple class for serial interface RS232
// for linux and windows without threads etc.
// #define LINUX_ 1 // for linux use, if not defined -> windows
// Hardware: (1) RS232 or USB-Serial-Adapter
// Released into the public domain.
// See also
// http://www.teuniz.net/RS-232/
// http://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html
// http://man7.org/linux/man-pages/man3/termios.3.html
// http://msdn.microsoft.com/en-us/library/windows/desktop/aa363145%28v=vs.85%29.aspx
// http://technet.microsoft.com/en-us/library/cc732236.aspx
#include "C_X232.h"

// *************************************************************
//    constructor & co
// *************************************************************
//_____constructor______________________________________________
X232::X232() {
 initX232();
 setup();
}

X232::X232(String device) {
 initX232();
 device_=device;
 setup();
}

X232::X232(String device, long baudrate) {
 initX232();
 device_=device;
 baud_  =baudrate;
 setup();
}

X232::X232(String device, long baudrate, String mode) {
 initX232();
 device_=device;
 baud_  =baudrate;
 mode_  =mode;
 setup();
}

//_____init command properties__________________________________
void X232::initX232()
{
 device_=X232_DEVICE;
 baud_  =X232_BAUDRATE;
 mode_  =X232_MODE;
 nolfcr_=false;
}

//_____setup____________________________________________________
void X232::setup() {
 xopen();
 xflush();
 xclose();
}

// *************************************************************
//    Setter and Getter methods
// *************************************************************

// *************************************************************
//    worker methods   - for both: linux AND windows
// *************************************************************
//_____try to open serial interface 3x__________________________
// needs up to 100ms
// return: 0=OK
int X232::xopen() {
 if(xopen1()==0) return 0;             // serial ok
 delay(48);
 int ret=xopen1();
 if(ret>=0) return ret;                // ok or no cmd lines
 delay(48);
 return(xopen1());                     // last trial
}

// *************************************************************
//    worker methods   - LINUX - LINUX - LINUX - LINUX - LINUX -
// *************************************************************
#if defined(LINUX_)

//_____open serial interface____________________________________
// return: 0=OK
int X232::xopen1() {
 speed_t baudB;
 int     status;
 //-----check baudrate------------------------------------------
 switch(baud_)
 {
  case      50: baudB = B50; break;
  case      75: baudB = B75; break;
  case     110: baudB = B110; break;
  case     134: baudB = B134; break;
  case     150: baudB = B150; break;
  case     200: baudB = B200; break;
  case     300: baudB = B300; break;
  case     600: baudB = B600; break;
  case    1200: baudB = B1200; break;
  case    1800: baudB = B1800; break;
  case    2400: baudB = B2400; break;
  case    4800: baudB = B4800; break;
  case    9600: baudB = B9600; break;
  case   19200: baudB = B19200; break;
  case   38400: baudB = B38400; break;
  case   57600: baudB = B57600; break;
  case  115200: baudB = B115200; break;
  case  230400: baudB = B230400; break;
  case  460800: baudB = B460800; break;
  case  500000: baudB = B500000; break;
  case  576000: baudB = B576000; break;
  case  921600: baudB = B921600; break;
  case 1000000: baudB = B1000000; break;
  case 1152000: baudB = B1152000; break;
  case 1500000: baudB = B1500000; break;
  case 2000000: baudB = B2000000; break;
  case 2500000: baudB = B2500000; break;
  case 3000000: baudB = B3000000; break;
  case 3500000: baudB = B3500000; break;
  case 4000000: baudB = B4000000; break;
  default     : return -3;          // invalid baudrate
 }
 //-----check mode----------------------------------------------
 int cbits=CS8,
     cpar=0,
     ipar=IGNPAR,
     bstop=0;
 if(mode_.length()!=3) return -4;     // invalid mode
 switch(mode_[0])                     // number of data bits
 {
  case '8': cbits = CS8; break;
  case '7': cbits = CS7; break;
  case '6': cbits = CS6; break;
  case '5': cbits = CS5; break;
  default: return -5;                  // invalid number data bits
 }
 switch(mode_[1])                      // parity bit
 {
  case 'N': case 'n': cpar = 0; ipar = IGNPAR; break;
  case 'E': case 'e': cpar = PARENB; ipar = INPCK; break;
  case 'O': case 'o': cpar = (PARENB | PARODD); ipar = INPCK; break;
  default : return -6;                 // invalid parity
 }
 switch(mode_[2])                      // number stop bits
 {
  case '1': bstop = 0; break;
  case '2': bstop = CSTOPB; break;
  default : return -7;                 // invalid number stop bits
 }
 //-----try to open serial interface----------------------------
 fd=open(device_.c_str(), O_RDWR|O_NOCTTY|O_NDELAY|O_NONBLOCK);
 if(fd==-1) return -1;
 //--lock access so that another process can't also use the port
 if(flock(fd, LOCK_EX | LOCK_NB) != 0)
 {//.....another process has locked the com port................
  close(fd);                           // close port
  return -8;
 }
 //-----get and modify current options--------------------------
 if(tcgetattr(fd, &old_port_settings)==-1)
 {//.....unable to read port settings...........................
  flock(fd, LOCK_UN);                  // free port for others
  close(fd);                           // close port
  return -9;
 }
 //-----clear struct (!) for new port settings------------------
 memset(&new_port_settings, 0, sizeof(new_port_settings));
 new_port_settings.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
 new_port_settings.c_iflag = ipar;
 new_port_settings.c_oflag = 0;
 new_port_settings.c_lflag = 0;
 new_port_settings.c_cc[VMIN] = 0;      // block until n bytes received
 new_port_settings.c_cc[VTIME] = 1;     // block 0,1sec (0*100ms)
 cfsetispeed(&new_port_settings, baudB);
 cfsetospeed(&new_port_settings, baudB);
 //-----set new port settings-----------------------------------
 if(tcsetattr(fd, TCSANOW, &new_port_settings)==-1)
 {//.....unable to write port settings..........................
  flock(fd, LOCK_UN);                  // free port for others
  close(fd);                           // close port
  return -10;
 }
 //-----turn on control lines DTR and RTS-----------------------
 if(ioctl(fd, TIOCMGET, &status) == -1)
 {//.....unable to get port status..............................
  tcsetattr(fd, TCSANOW, &old_port_settings);
  flock(fd, LOCK_UN);                  // free port for others
  close(fd);                           // close port
  return -11;
 }
 status |= TIOCM_DTR;                  // turn on DTR
 status |= TIOCM_RTS;                  // turn on RTS
 if(ioctl(fd, TIOCMSET, &status) == -1)
 {//.....unable to set port status..............................
  tcsetattr(fd, TCSANOW, &old_port_settings);
  flock(fd, LOCK_UN);                  // free port for others
  close(fd);                           // close port
  return -12;
 }
 delay(10);                            // wait 10ms
 return X232_OK;
}

//_____close port_______________________________________________
int X232::xclose()
{
 int status=0;
 //-----turn off control lines DTR and RTS----------------------
 if(ioctl(fd, TIOCMGET, &status) == -1)
 {//.....unable to get port status..............................
 }
 status &= ~TIOCM_DTR;                 // turn off DTR
 status &= ~TIOCM_RTS;                 // turn off RTS
 if(ioctl(fd, TIOCMSET, &status) == -1)
 {//.....unable to set port status..............................

 }
 //-----set old port settings-----------------------------------
 tcsetattr(fd, TCSANOW, &old_port_settings);
 flock(fd, LOCK_UN);                   // free port for others
 close(fd);                            // close port
 fd=-1;                                //file descriptor invalid
 return X232_OK;
}

//_____send a character to serial_______________________________
// return >=0 OK (ascii number), <0 on error
int X232::xputc(const char c)
{
 if(fd<0) return -1;
 int n = write(fd, &c, 1);
 if(n < 0)
 {
  if(errno == EAGAIN) return -2;       // temporarily unavailable
  else return -3;
 }
 return c;
}


//_____send a character buffer to serial________________________
// return >0 OK (number of written chars), <0 on error
int X232::xputs(const char *buf, int size)
{
 if(fd<0) return -1;
 int n=0;
 int ret;
 for(int i=0; i<size; i++)
 {
  ret=xputc(buf[i]);
  if(ret<0) { delay(2); ret=xputc(buf[i]); }
  if(ret>=0) n++;
 }
 //n = write(fd, buf, size);
 //if(n <= 0)
 //{
 // if(errno == EAGAIN) return -2;
 // else return -3;
 //}
 return n;
}

//_____send a string to serial__________________________________
// return >0 OK (number of written chars), <0 on error
int X232::xprint(const String s1) {
 if(fd<0) return -1;
 char s[s1.length()+1];
 strcpy(s,s1.c_str());
 return xputs(s,s1.length());
}

//_____send a string line to serial_____________________________
// return >0 OK (number of written chars), <0 on error
int X232::xprintln(String s1) {
 s1=s1+EOL;
 return(xprint(s1));
}

//_____format input and send it to serial_______________________
// return >0 OK (number of written chars), <0 on error
int X232::xprintf(const char *message, ...) {
 if(fd<0) return -1;
 va_list argp;
 char buf[1024];
 va_start(argp, message);
 vsnprintf(buf, 1023, message, argp);
 va_end(argp);
 int n = write(fd, buf, strlen(buf));
 if(n < 0)
 {
  if(errno == EAGAIN) return -2;
  else return -3;
 }
 return n;
}

//_____are bytes available?_____________________________________
// return: number of bytes available to be read from serial port
int X232::xdataAvail()
{
 int ret;
 if(ioctl(fd, FIONREAD, &ret)== -1) return -1 ;
 return ret;
}

//_____read a character from serial port________________________
// return: >=0 character, -1 on error
int X232::xgetc()
{
 int ret=-1;
 unsigned char c;
 int n=xdataAvail();
 if(n>0) {
  while((ret=read(fd,&c,1))==-1 && errno==EINTR) {}
  if(ret==1){
   if(nolfcr_) {if(c=='\r') c='^'; if(c=='\n') c='~'; }
   ret=((int)c) & 0xFF;
  }
 }
 return ret;
}

//_____read <size> characters from serial port__________________
// return: number of read chars (-1 on error)
int X232::xgets(char *buf, int size)
{
 if(fd<0) return -1;
 int n=read(fd, buf, size);
 if(n < 0) {
  if(errno == EAGAIN)  return 0;
 }
 return n;
}

//_____clear io buffer__________________________________________
void X232::xflush()   { tcflush(fd, TCIOFLUSH); }
void X232::xflushRX() { tcflush(fd, TCIFLUSH); }
void X232::xflushTX() { tcflush(fd, TCOFLUSH); }

//_____set output line RTS______________________________________
// return: 0=OK, <0 error
int X232::xsetRTS(bool on_)
{
 int status;
 if(fd<0) return -1;                   // port not open
 if(ioctl(fd, TIOCMGET, &status) == -1)
  return -2;                           // no port status
 if(on_) status |= TIOCM_RTS;          // turn on RTS
 else status &= ~TIOCM_RTS;            // turn off RTS
 if(ioctl(fd, TIOCMSET, &status) == -1)
  return -3;                           // port status not set
 return 0;
}

//_____set output line DTR______________________________________
// return: 0=OK, <0 error
int X232::xsetDTR(bool on_)
{
 int status;
 if(fd<0) return -1;                   // port not open
 if(ioctl(fd, TIOCMGET, &status) == -1)
  return -2;                           // no port status
 if(on_) status |= TIOCM_DTR;          // turn on DTR
 else status &= ~TIOCM_DTR;            // turn off DTR
 if(ioctl(fd, TIOCMSET, &status) == -1)
  return -3;                           // port status not set
 return 0;
}

//_____read input line DCD______________________________________
// return: 0 or 1 (on error -1 or -2)
int X232::xisDCD()
{
 int status;
 if(fd<0) return -1;                   // port not open
 if(ioctl(fd, TIOCMGET, &status) == -1)
  return -2;                           // no port status
 if(status&TIOCM_CAR) return(1);
 else return(0);
}

//_____read input line CTS______________________________________
// return: 0 or 1 (on error -1 or -2)
int X232::xisCTS()
{
 int status;
 if(fd<0) return -1;                   // port not open
 if(ioctl(fd, TIOCMGET, &status) == -1)
  return -2;                           // no port status
 if(status&TIOCM_CTS) return(1);
 else return(0);
}

//_____read input line DSR______________________________________
// return: 0 or 1 (on error -1 or -2)
int X232::xisDSR()
{
 int status;
 if(fd<0) return -1;                   // port not open
 if(ioctl(fd, TIOCMGET, &status) == -1)
  return -2;                           // no port status
 if(status&TIOCM_DSR) return(1);
 else return(0);
}

//_____read input line RI_______________________________________
// return: 0 or 1 (on error -1 or -2)
int X232::xisRI()
{
 int status;
 if(fd<0) return -1;                   // port not open
 if(ioctl(fd, TIOCMGET, &status) == -1)
  return -2;                           // no port status
 if(status&TIOCM_RNG) return(1);
 else return(0);
}

// *************************************************************
//    worker methods   - WINDOWS - WINDOWS - WINDOWS - WINDOWS -
// *************************************************************
#else  /* windows */

//_____open serial interface____________________________________
// return: 0=OK <0 error, >0 no control lines
int X232::xopen1() {
 //-----check baudrate------------------------------------------
 switch(baud_)
 {
  case     110: strcpy(mode_str, "baud=110"); break;
  case     300: strcpy(mode_str, "baud=300"); break;
  case     600: strcpy(mode_str, "baud=600"); break;
  case    1200: strcpy(mode_str, "baud=1200"); break;
  case    2400: strcpy(mode_str, "baud=2400"); break;
  case    4800: strcpy(mode_str, "baud=4800"); break;
  case    9600: strcpy(mode_str, "baud=9600"); break;
  case   19200: strcpy(mode_str, "baud=19200"); break;
  case   38400: strcpy(mode_str, "baud=38400"); break;
  case   57600: strcpy(mode_str, "baud=57600"); break;
  case  115200: strcpy(mode_str, "baud=115200"); break;
  case  128000: strcpy(mode_str, "baud=128000"); break;
  case  256000: strcpy(mode_str, "baud=256000"); break;
  case  500000: strcpy(mode_str, "baud=500000"); break;
  case 1000000: strcpy(mode_str, "baud=1000000"); break;
  default     : return -3;          // invalid baudrate
 }
 //-----check mode----------------------------------------------
 if(mode_.length()!=3) return -4;     // invalid mode
 switch(mode_[0])                     // number of data bits
 {
  case '8': strcat(mode_str, " data=8"); break;
  case '7': strcat(mode_str, " data=7"); break;
  case '6': strcat(mode_str, " data=6"); break;
  case '5': strcat(mode_str, " data=5"); break;
  default: return -5;                  // invalid number data bits
 }
 switch(mode_[1])
 {
  case 'N': case 'n': strcat(mode_str, " parity=n"); break;
  case 'E': case 'e': strcat(mode_str, " parity=e"); break;
  case 'O': case 'o': strcat(mode_str, " parity=o"); break;
  default : return -6;
 }
 switch(mode_[2])                      // number stop bits
 {
  case '1': strcat(mode_str, " stop=1"); break;
  case '2': strcat(mode_str, " stop=2"); break;
  default : return -7;                 // invalid number stop bits
 }
 //-----turn on control lines DTR and RTS-----------------------
 strcat(mode_str, " dtr=on rts=on");
 //-----try to open serial interface----------------------------
 fd=CreateFileA(device_.c_str(),
    GENERIC_READ|GENERIC_WRITE,
    0,                                 // no share
    NULL,                              // no security
    OPEN_EXISTING,
    0,                                 // no threads
    NULL);                             // no templates
//printf("device=%s, fd=%d\n",device_.c_str(),fd);
 if(fd==INVALID_HANDLE_VALUE) return -1;
 DCB port_settings;
 memset(&port_settings, 0, sizeof(port_settings)); //clear new struct
 port_settings.DCBlength = sizeof(port_settings);
 if(!BuildCommDCBA(mode_str, &port_settings))
 {//.....unable to set comport dcb settings.....................
  CloseHandle(fd);
  return -8;
 }

 if(!SetCommState(fd, &port_settings))
 {//.....unable to set comport cfg settings.....................
  //CloseHandle(fd);
  return 1;
 }
  COMMTIMEOUTS Cptimeouts;
  Cptimeouts.ReadIntervalTimeout         = MAXDWORD;
  Cptimeouts.ReadTotalTimeoutMultiplier  = 0;
  Cptimeouts.ReadTotalTimeoutConstant    = 0;
  Cptimeouts.WriteTotalTimeoutMultiplier = 0;
  Cptimeouts.WriteTotalTimeoutConstant   = 0;
 if(!SetCommTimeouts(fd, &Cptimeouts))
 {//.....unable to set comport time-out settings................
  //CloseHandle(fd);
  return 2;
 }
 return X232_OK;
}

//_____close port_______________________________________________
int X232::xclose(){
 if(fd==INVALID_HANDLE_VALUE) return -1;
 CloseHandle(fd);
 return X232_OK;
}

//_____send a character to serial_______________________________
// return >=0 OK (ascii number), <0 on error
int X232::xputc(const char c)
{
 if(fd==INVALID_HANDLE_VALUE) return -1;
 DWORD n;
 //char buf[2]={0,0};
 //buf[0]=c;
 //WriteFile (hCom, &ucMsg, 1, &iBytesWritten, NULL); // Senden eines Bytes
 //WriteFile(fd, &c, 1, (LPDWORD)((void *)&n), NULL);
 WriteFile(fd, &c, 1, &n, NULL);
 //WriteFile(fd, buf, 1, (LPDWORD)((void *)&n), NULL);
 if(n<0)  return -2;
 return c;
}

//_____send a character buffer to serial________________________
// return >0 OK (number of written chars), <0 on error
int X232::xputs(const char *buf, int size)
{
 DWORD n;
 if(fd==INVALID_HANDLE_VALUE) return -1;
 //if(WriteFile(fd, buf, size, (LPDWORD)((void *)&n), NULL))
 if(WriteFile(fd, buf, size, &n, NULL))
  return n;
 return -2;
}

//_____send a string to serial__________________________________
// return >0 OK (number of written chars), <0 on error
int X232::xprint(const String s1) {
 if(fd==INVALID_HANDLE_VALUE) return -1;
 char s[s1.length()+1];
 strcpy(s,s1.c_str());
 return xputs(s,s1.length());
}

//_____send a string line to serial_____________________________
// return >0 OK (number of written chars), <0 on error
int X232::xprintln(String s1) {
 s1=s1+EOL;
 return(xprint(s1));
}

//_____format input and send it to serial_______________________
// return >0 OK (number of written chars), <0 on error
int X232::xprintf(const char *message, ...) {
 if(fd==INVALID_HANDLE_VALUE) return -1;
 DWORD n;
 va_list argp;
 char buf[1024];
 va_start(argp, message);
 vsnprintf(buf, 1023, message, argp);
 va_end(argp);
 //if(WriteFile(fd, buf, strlen(buf), (LPDWORD)((void *)&n), NULL))
 if(WriteFile(fd, buf, strlen(buf), &n, NULL))
  return n;
 return -2;
}

//_____are bytes available?_____________________________________
// return: number of bytes available to be read from serial port
int X232::xdataAvail()
{
 if(fd==INVALID_HANDLE_VALUE) return -1;
 DWORD commerr;
 COMSTAT comstat;
 ClearCommError(fd, &commerr, &comstat);
 if(comstat.cbInQue == 0) return 0;    // buffer empty
 return 1;
}

//_____read a character from serial port________________________
// return: >=0 character, <0 on error
int X232::xgetc()
{
 if(fd==INVALID_HANDLE_VALUE) return -1;
 DWORD n=0;
 char c;
 if(xdataAvail()>0) {
  ReadFile(fd, &c, 1, &n, NULL);
 }
 if(n<1) return -2;
 if(nolfcr_) {if(c=='\r') c='^'; if(c=='\n') c='~'; }
 return ((int)c) & 0xFF;
}

//_____read <size> characters from serial port__________________
// return: number of read chars (-1 on error)
int X232::xgets(char *buf, int size)
{
 if(fd==INVALID_HANDLE_VALUE) return -1;
 DWORD n;
 ReadFile(fd, buf, size, &n, NULL);
 return n;
}

//_____clear io buffer__________________________________________
void X232::xflush()   {
 PurgeComm(fd, PURGE_RXCLEAR | PURGE_RXABORT);
 PurgeComm(fd, PURGE_TXCLEAR | PURGE_TXABORT);
}

void X232::xflushRX() {
 PurgeComm(fd, PURGE_RXCLEAR | PURGE_RXABORT);
}

void X232::xflushTX() {
 PurgeComm(fd, PURGE_TXCLEAR | PURGE_TXABORT);
}

//_____set output line RTS______________________________________
// return: 0=OK, <0 error
int X232::xsetRTS(bool on_)
{
 if(fd==INVALID_HANDLE_VALUE) return -1;
 if(on_) EscapeCommFunction(fd, SETRTS);
    else EscapeCommFunction(fd, CLRRTS);
 return 0;
}

//_____set output line DTR______________________________________
// return: 0=OK, <0 error
int X232::xsetDTR(bool on_)
{
 if(fd==INVALID_HANDLE_VALUE) return -1;
 if(on_) EscapeCommFunction(fd, SETDTR);
    else EscapeCommFunction(fd, CLRDTR);
 return 0;
}

//_____read input line DCD______________________________________
// return: 0 or 1 (on error -1 or -2)
int X232::xisDCD()
{
 if(fd==INVALID_HANDLE_VALUE) return -1;
 int status;
 GetCommModemStatus(fd, (LPDWORD)((void *)&status));
 if(status&MS_RLSD_ON) return(1);
 else return(0);
}

//_____read input line CTS______________________________________
// return: 0 or 1 (on error -1 or -2)
int X232::xisCTS()
{
 if(fd==INVALID_HANDLE_VALUE) return -1;    // port not open
 int status;
 GetCommModemStatus(fd, (LPDWORD)((void *)&status));
 if(status&MS_CTS_ON) return(1);
 else return(0);
}

//_____read input line DSR______________________________________
// return: 0 or 1 (on error -1 or -2)
int X232::xisDSR()
{
 if(fd==INVALID_HANDLE_VALUE) return -1;    // port not open
 int status;
 GetCommModemStatus(fd, (LPDWORD)((void *)&status));
 if(status&MS_DSR_ON) return(1);
 else return(0);
}

//_____read input line RI_______________________________________
// return: 0 or 1 (on error -1 or -2)
int X232::xisRI()
{
 if(fd==INVALID_HANDLE_VALUE) return -1;    // port not open
 int status;
 GetCommModemStatus(fd, (LPDWORD)((void *)&status));
 if(status&MS_RING_ON) return(1);
 else return(0);
}

#endif
