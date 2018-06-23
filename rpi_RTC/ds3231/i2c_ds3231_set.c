// i2c_ds3231_set.c, 3.7.2016-1.11.2016, Karl Hartinger
// gcc i2c_ds3231_set.c utils_ds3231.c -o i2c_ds3231_set -l wiringPi
#include "utils_ds3231.h"

//_____MAIN FUNCTION____________________________________________
int main(int argc, char **argv)
{
 int fh_;
 char i2c_address=I2C_ADDRESS;
 char sdate_[10];
 char stime_[10];
 char sday_[11];
 datetime_t datetime_;
 int  ret=NO_ARGS;               // return value
 char *pdata;                     // pointer to arg data   
 int nr_;
 int temp_;
 //-----check argument(s)---------------------------------------
 if(argc>1)
 {
  //.....get date...............................................
  strncpy(sdate_,argv[1],7);
  ret=checkdate(sdate_, &datetime_);
  if((ret==_OK_)&&(argc>2))
  {
   strncpy(stime_,argv[2],7);
   ret=checktime(stime_, &datetime_);
  }
  if(argc>3) ret=TOO_MANY_ARGS;
 }
 //-----if params not ok: help text-----------------------------
 if(ret!=_OK_)
 {
  printf("Raspberry Pi: Set date and time on RTC DS3231\n");
  printf("Author:   Karl Hartinger, 3.7.2016-1.11.2016\n");
  printf("Usage:    i2c_ds3231_set yymmdd hhiiss\n");
  printf("          yy..year (00..99), mm..month   (01..12), dd..day (01..31),\n"); 
  printf("          hh..hour (00..23), ii...minute (00..59), ss...seconds\n");
  printf("Example:  i2c_ds3231_set $(date +\"%%y%%m%%d %%H%%M%%S\")\n");
  return(ret);
 }
 //-----setup wiringPi------------------------------------------
 if (wiringPiSetup () == -1)
 {
  if(DEBUG) printf("ERROR: wiringPiSetup failed\n"); 
  return ERROR_WIRINGPI;
 }
 if ((fh_=wiringPiI2CSetup(i2c_address)) == -1)
 {
  if(DEBUG) printf("ERROR: wiringPiI2CSetup(%02X) failed\n",I2C_ADDRESS);
  return ERROR_I2C;
 }
 //-----get number day of week----------------------------------
 nr_=dayofweek(datetime_.dd, datetime_.mm,2000+datetime_.yy);
 datetime_.nr=nr_;
 getweekday(nr_, sday_, 0);
 if(DEBUG) printf("Datum  : %02d.%02d.%02d\n",datetime_.dd, datetime_.mm,datetime_.yy);
 if(DEBUG) printf("Uhrzeit: %02d.%02d.%02d\n",datetime_.hh, datetime_.ii,datetime_.ss);
 if(DEBUG) printf("Wochentag %d => %s\n", nr_, sday_);
 //-----write to RTC--------------------------------------------
 wiringPiI2CWriteReg8(fh_, 6, bin2bcd(datetime_.yy));
 wiringPiI2CWriteReg8(fh_, 5, bin2bcd(datetime_.mm));
 wiringPiI2CWriteReg8(fh_, 4, bin2bcd(datetime_.dd));
 wiringPiI2CWriteReg8(fh_, 3, bin2bcd(datetime_.nr));
 wiringPiI2CWriteReg8(fh_, 2, bin2bcd(datetime_.hh));
 wiringPiI2CWriteReg8(fh_, 1, bin2bcd(datetime_.ii));
 wiringPiI2CWriteReg8(fh_, 0, bin2bcd(datetime_.ss));
 return _OK_;
}