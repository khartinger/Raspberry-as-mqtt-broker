//f_dayofweek.c

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