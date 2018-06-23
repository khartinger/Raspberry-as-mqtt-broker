// utils_ds3231.h, 3.7.2016-19.12.2016, karl.hartinger@gmail.com
#ifndef _UTILS_DS3231_H_
 #define _UTILS_DS3231_H_
 #ifdef  __cplusplus
  extern "C" {
 #endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>

//_____common defines___________________________________________
#define DEBUG                1
#define I2C_ADDRESS          0x68
#define _OK_                 0
#define ERROR_WIRINGPI       1
#define ERROR_I2C            2
#define NO_ARGS              -1
#define WRONG_DATE           -2
#define WRONG_TIME           -3
#define WRONG_I2C_ADDRESS    -5
#define TOO_MANY_ARGS        -6

//_____date and time as int values______________________________
typedef struct 
{
 int yy, mm, dd, nr, hh, ii, ss;
}datetime_t;

//_____convert binary to bcd (binary coded decimal)_____________
// 0 -> 0x00, 10 -> 0x10 (16), 99 -> 0x99 (153)
// Error: return 0xFF
unsigned char bin2bcd(unsigned char bin_);

//_____convert bcd (binary coded decimal) to binary_____________
// 0x00 -> 0x00, 0x10 -> 16, 0x99 -> 99
// Error: return 0xFF
unsigned char bcd2bin(unsigned char bcd_);

//_____Date yyMMdd?_____________________________________________
int checkdate(char* sdate, datetime_t *datetime);

//_____Time hhmmss?_____________________________________________
int checktime(char* stime, datetime_t *datetime);

//_____Wochentag zu Datum: 1=Sonntag, 2=Montag,...7=Samstag_____
// Rarameter: Tag, Monat, Jahr; Jahr unbedingt vierstellig
// Rueckgabe: 0 = Datum ungueltig z.B. 29.2.2007, 32.12.2007
//           -1 = Jahr kleiner als 1583 oder groesser als 9999
int dayofweek(int d, int m, int y);

//_____Wochentag als Text_______________________________________
// nr_ 1..7 ergibt 1=Sonntag, 2=Montag, ...
// kurz_ungleich 0: Wochentag nur 2 Buchstaben, sonst Langtext
void getweekday(int nr_, char* weekday, int kurz_);

//_____make DateLong YYYY-mm-dd hh:ii:ss________________________
// Return: 0=OK (string formatted) or 1 on error
int makeDateLong(char* sDateLong, datetime_t datetime);

//_____make date format mmddhhiiyy.ss___________________________
// Return: 0=OK (string formatted) or 1 on error
int makeDateDate(char* sDateLong, datetime_t datetime);

 #ifdef  __cplusplus
 }
 #endif
#endif //_UTILS_LCD_H_