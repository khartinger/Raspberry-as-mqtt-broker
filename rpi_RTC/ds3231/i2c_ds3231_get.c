// i2c_ds3231_get.c, 6.7.2016-1.11.2016, Karl Hartinger
// gcc i2c_ds3231_get.c utils_ds3231.c -o i2c_ds3231_get -l wiringPi
#include "utils_ds3231.h"

//_____MAIN FUNCTION____________________________________________
int main(int argc, char **argv)
{
 int fh_;
 char i2c_address=I2C_ADDRESS;
 char sDateLong_[]="2000-01-01 00:00:00";
 datetime_t datetime_;
 int  ret=NO_ARGS;               // return value
 int  dateformat=0;
 //-----check argument(s)---------------------------------------
 if(argc>1)
 {
  if(argv[1][0]=='1') { dateformat=1; ret=_OK_; }
  if(argv[1][0]=='2') { dateformat=2; ret=_OK_; }
 }
 //-----if params not ok: help text-----------------------------
 if(ret!=_OK_)
 {
  printf("Raspberry Pi: Get date and time from RTC DS3231\n");
  printf("Author:   Karl Hartinger, 3.7.2016-30.10.2016\n");
  printf("Usage:    i2c_ds3231_get [-h]\n");
  printf("Format:   1 = YYYY-mm-dd hh:ii:ss\n");
  printf("          2 = mmddhhiiyy.ss (for linux date -u)\n");
  printf("Example:  sudo date +\"%%F %%X\" --set \"$(sudo ./i2c_ds3231_get 1)\"\n");
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
 //-----read from RTC-------------------------------------------
 datetime_.yy=bcd2bin(wiringPiI2CReadReg8(fh_, 6));
 datetime_.mm=bcd2bin(wiringPiI2CReadReg8(fh_, 5));
 datetime_.dd=bcd2bin(wiringPiI2CReadReg8(fh_, 4));
 datetime_.nr=bcd2bin(wiringPiI2CReadReg8(fh_, 3));
 datetime_.hh=bcd2bin(wiringPiI2CReadReg8(fh_, 2));
 datetime_.ii=bcd2bin(wiringPiI2CReadReg8(fh_, 1));
 datetime_.ss=bcd2bin(wiringPiI2CReadReg8(fh_, 0));
 //if(DEBUG) printf("Zahlenwerte: %d.%d.%d %02d:%02d:%02d\n", datetime_.dd,datetime_.mm,datetime_.yy,datetime_.hh,datetime_.ii,datetime_.ss);
 if(dateformat==2)
  makeDateDate(sDateLong_, datetime_);
 else
  makeDateLong(sDateLong_, datetime_);
 printf("%s\n",sDateLong_);
 return _OK_;
}