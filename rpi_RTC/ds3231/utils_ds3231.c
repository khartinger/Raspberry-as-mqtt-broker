// utils_ds3231.c, 3.7.2016-30.10.2016, karl.hartinger@gmail.com
#include "utils_ds3231.h"

//_____convert binary to bcd (binary coded decimal)_____________
// 0 -> 0x00, 10 -> 0x10 (16), 99 -> 0x99 (153)
// Error: return 0xFF
unsigned char bin2bcd(unsigned char bin_)
{
 unsigned char z_, e_;
 if((bin_<0)||(bin_>99)) return 0xFF;
 z_ = (int) (bin_/10);
 e_ = bin_ - 10*z_;
 return z_*16 + e_;
}

//_____convert bcd (binary coded decimal) to binary_____________
// 0x00 -> 0x00, 0x10 -> 16, 0x99 -> 99
// Error: return 0xFF
unsigned char bcd2bin(unsigned char bcd_)
{
 unsigned char z_, e_;
 z_= bcd_ >> 4;
 e_= bcd_ & 0x0F;
 if((z_>9)||(e_>9)) return 0xFF;
 return (z_*10 + e_);
}

//_____Date yyMMdd?_____________________________________________
int checkdate(char* sdate, datetime_t *datetime)
{
 int mtag[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
 int y4=0,y,m,d;                  // leapyear?, year, month, day
 char sTemp[3]="00";
 if(strlen(sdate)!=6) return WRONG_DATE;
 //-----check year----------------------------------------------
 sTemp[0]=sdate[0];
 sTemp[1]=sdate[1];
 y=atoi(sTemp);
 if ((y%4==0 && y%100!=0) || y%400==0) y4=1; //Schaltjahrcheck
 //-----check month---------------------------------------------
 sTemp[0]=sdate[2];
 sTemp[1]=sdate[3];
 m=atoi(sTemp);
 if((m<1)||(m>12)) return WRONG_DATE;
 //-----check day-----------------------------------------------
 sTemp[0]=sdate[4];
 sTemp[1]=sdate[5];
 d=atoi(sTemp);
 if ((d<1) || (d>mtag[m]+y4*(m==2))) return WRONG_DATE;
 datetime->yy=y;
 datetime->mm=m;
 datetime->dd=d;
 return _OK_;
}

//_____Time hhmmss?_____________________________________________
int checktime(char* stime, datetime_t *datetime)
{
 int h,m,s;
 char sTemp[3]="00";
 //-----check hour----------------------------------------------
 sTemp[0]=stime[0];
 sTemp[1]=stime[1];
 h=atoi(sTemp);
 if((h<0)||(h>23)) return WRONG_TIME;
 //-----check minute--------------------------------------------
 sTemp[0]=stime[2];
 sTemp[1]=stime[3];
 m=atoi(sTemp);
 if((m<0)||(m>59)) return WRONG_TIME;
 //-----check second--------------------------------------------
 sTemp[0]=stime[4];
 sTemp[1]=stime[5];
 s=atoi(sTemp);
 if((s<0)||(s>59)) return WRONG_TIME;
 datetime->hh=h;
 datetime->ii=m;
 datetime->ss=s;
 return _OK_;
}

//_____Wochentag zu Datum: 1=Sonntag, 2=Montag,...7=Samstag_____
// Rarameter: Tag, Monat, Jahr; Jahr unbedingt vierstellig
// Rueckgabe: 0 = Datum ungueltig z.B. 29.2.2007, 32.12.2007
//           -1 = Jahr kleiner als 1583 oder groesser als 9999
int dayofweek(int d, int m, int y)
{
 int s=0;                                   //Tage pro Monat:
 int mtag[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
 if ((y%4==0 && y%100!=0) || y%400==0) s=1; //Schaltjahrcheck
 if (y<1583 || y>9999) return(-1);          //Jahr gueltig?
 if (m<1 || m>12) return(0);                //Monat gueltig?
 if (d<1 || d>mtag[m]+s*(m==2)) return(0);  //Tag gueltig?
 if (m < 3) { m += 13; y--; } else  m++;    //Jahresanfang Maerz
 s = d + 26*m/10 + y+y/4-y/100+y/400 + 6;   //Berechnung
 return(s % 7 + 1);                         //Ergebnis anpassen
}

//_____Wochentag als Text_______________________________________
void getweekday(int nr_, char* weekday, int kurz_)
{
 switch(nr_)
 {
  case 1: if(kurz_) strcpy(weekday, "SO"); 
          else strcpy(weekday, "Sonntag");
          break;
  case 2: if(kurz_) strcpy(weekday, "MO"); 
          else strcpy(weekday, "Montag");
          break;
  case 3: if(kurz_) strcpy(weekday, "DI"); 
          else strcpy(weekday, "Dienstag");
          break;
  case 4: if(kurz_) strcpy(weekday, "MI"); 
          else strcpy(weekday, "Mittwoch");
          break;
  case 5: if(kurz_) strcpy(weekday, "DO"); 
          else strcpy(weekday, "Donnerstag");
          break;
  case 6: if(kurz_) strcpy(weekday, "FR"); 
          else strcpy(weekday, "Freitag");
          break;
  case 7: if(kurz_) strcpy(weekday, "SA"); 
          else strcpy(weekday, "Samstag");
          break;
  default: if(kurz_) strcpy(weekday, "??"); 
           else strcpy(weekday, "Fehler!");
  strcpy(weekday, "FEHLER!");
 }
}

//_____make DateLong YYYY-mm-dd hh:ii:ss________________________
// Return: 0=OK (string formatted) or 1 on error
int makeDateLong(char* sDateLong, datetime_t datetime)
{
 if(strlen(sDateLong)<19) return 1;
 sprintf(sDateLong, "%4d-%02d-%02d %02d:%02d:%02d",
  2000+datetime.yy, datetime.mm, datetime.dd,
  datetime.hh, datetime.ii, datetime.ss);
 return 0;
}

//_____make date format mmddhhiiyy.ss___________________________
int makeDateDate(char* sDateLong, datetime_t datetime)
// Return: 0=OK (string formatted) or 1 on error
{
 if(strlen(sDateLong)<19) return 1;
 sprintf(sDateLong, "%02d%02d%02d%02d%02d.%02d",
  datetime.mm, datetime.dd, datetime.hh,
  datetime.ii, datetime.yy, datetime.ss);
 return 0;
}