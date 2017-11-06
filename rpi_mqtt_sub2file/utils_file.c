/* utils_file.c, 13.8.2014-8.3.2015, Karl Hartinger           */
#include "utils_file.h"

//=====Special data types=======================================
type_line last_error;
int last_errno;

//_____does file exist?_________________________________________
// returns 1=yes, 0=no
int isfile(char *filename_)
{
 FILE *fp;
 if( (fp=fopen(filename_, "r")) == NULL) return 0;
 fclose(fp);
 return 1;
}
 
//_____delete file______________________________________________
// returns 0=OK, <>0 on error
int deletefile(char *filename_)
{
 FILE *fp;
 if( (fp=fopen(filename_, "r")) == NULL) return 0;
 fclose(fp);
 char temp[FILENAME_MAX+4]="rm ";
 strcat(temp, filename_);
 return(system(temp));
}

//_____writes a line (string) in a file_________________________
// old values will be deleted
// returns 0=OK, 1 on error
int writeline(char *filename_, char *line_)
{
 return (saveline(filename_, "w", line_));
}

//_____insert (append) a new line in log file___________________
// returns 0=OK, 1 on error                                   
int insertline(char *filename_, char *line_)
{
 return (saveline(filename_, "a", line_));
}

//_____insert a new first line in log file______________________
// returns 0=OK, 1,2 or 3 on error
int insertfirstline(char *filename_, char *line_)
{
 FILE *fp1, *fp2;
 type_line line1_;
 char s[255];
 int ret;
 //-----delete temp file----------------------------------------
 if(isfile("temp.log")) deletefile("temp.log");
 //-----file does not exist yet---------------------------------
 if(isfile(filename_)!=1)
 {
  ret=saveline(filename_, "w", line_);
  return ret;
 }
 //-----copy old file to temp.log-------------------------------
 sprintf(s,"cp %s temp.log", filename_);
 system(s);
 ret=saveline(filename_, "w", line_);
 //-----open file-----------------------------------------------
 fp1=fopen("temp.log", "r");
 if( fp1 == NULL)
 {
  last_errno=errno;
  sprintf(last_error,"Fehler 1 - insertfirstline: %d %s\n",errno,strerror(errno));
  return 1;
 }
 fp2=fopen(filename_, "a");
 if( fp2 == NULL)
 {
  last_errno=errno;
  sprintf(last_error,"Fehler 2 - insertfirstline: %d %s\n",errno,strerror(errno));
  fclose(fp1);
  deletefile("temp.log");
  return 2;
 }
 //-----copy lines----------------------------------------------
 while( fgets(line1_, LINE_LEN_MAX, fp1) != NULL)
 {
  ret=fputs(line1_, fp2);
  if(ret<0)
  {
   last_errno=errno;
   sprintf(last_error,"Fehler 3 - insertfirstline: %d %s\n",errno,strerror(errno));
   fclose(fp2);
   fclose(fp1);
   deletefile("temp.log");
   return 3;
  }
 }
 fclose(fp2);
 fclose(fp1);
 deletefile("temp.log");
 return ret;
}

//_____get first line___________________________________________
// returns 0=OK, 1 on error
int getfirstline(char *filename_, char *line_)
{
 FILE *fp;
 if( (fp=fopen(filename_, "r")) == NULL) return 1;
 fgets(line_, LINE_LEN_MAX, fp);
 remove_lfcr(line_);
 fclose(fp);
 return 0;
}

//_____get last line____________________________________________
// returns 0=OK, 1 on error
int getlastline(char *filename_, char *line_)
{
 FILE *fp;
 if( (fp=fopen(filename_, "r")) == NULL) return 1;
 while( fgets(line_, LINE_LEN_MAX, fp) != NULL) ;
 remove_lfcr(line_);
 fclose(fp);
 return 0;
}

//_____show content of (text) file______________________________
// returns 0=OK, 1 on error
int showfile(char *filename_)
{
 FILE *fp;
 type_line line_;
 if( (fp=fopen(filename_, "r+")) == NULL) return 1;
 while( fgets(line_,LINE_LEN_MAX,fp) != NULL) 
  printf("%s",line_);
 fclose(fp);
 return 0;
}

//_____save a line to a file____________________________________
// type "w" makes a new file, "a" appends to a file
// returns 0=OK, 1 on error                                   
int saveline(char *filename_, char* type_, char *line_)
{
 int ret;
 FILE *fp;
 type_line line1_;
 //-----check open-type-----------------------------------------
 if((strcmp(type_,"w")!=0) && (strcmp(type_,"a")!=0) && (strcmp(type_,"a+")!=0))
 {
  last_errno=-1;
  sprintf(last_error,"writelines-1: Error type ist not \"w\" or \"a\"\n");
  return 1;
 }
 //-----open file-----------------------------------------------
 fp=fopen(filename_, type_);
 if( fp == NULL)
 {
  last_errno=errno;
  sprintf(last_error,"Fehler 1 - saveline: %d %s\n",errno,strerror(errno));
  return 1;
 }
 //-----write line to file--------------------------------------
 sprintf(line1_,"%s%s",line_,EOL);
 ret=fputs(line1_, fp);
 if(ret<0)
 {
  last_errno=errno;
  sprintf(last_error,"Fehler 2 - saveline: %d %s\n",errno,strerror(errno));
  fclose(fp); 
  return 1;
 }
 //-----close file----------------------------------------------
 ret=fclose(fp);
 if(ret<0)
 {
  last_errno=errno;
  sprintf(last_error,"Fehler 3 - saveline: %d %s\n",errno,strerror(errno));
  fclose(fp); 
  return 1;
 }
 return 0;
}

//_____save lines to a file_____________________________________
// type "w" makes a new file, "a" appends to a file
// returns 0=OK, 1 on error
int savelines(char *filename_, char* type_, int nr_, type_line lines_[])
{
 int i,ret;
 FILE *fp1;
 //-----check open-type-----------------------------------------
 if((strcmp(type_,"w")!=0) && (strcmp(type_,"a")!=0) && (strcmp(type_,"a+")!=0))
 {
  last_errno=-1;
  sprintf(last_error,"savelines-1: Error type ist not \"w\" or \"a\"\n");
  return 1;
 }
 //-----try to open file----------------------------------------
 fp1=fopen(filename_, type_);
 if(fp1==NULL) 
 {
  last_errno=errno;
  sprintf(last_error,"savelines-2: Error %d: %s\n",errno,strerror(errno));
  return 2;
 }
 //-----write lines to file-------------------------------------
 for(i=0; i<nr_; i++)
 {
  ret=fputs(lines_[i], fp1);
  if(ret<0)
  {
   last_errno=errno;
   sprintf(last_error,"savelines-3-%d Error %d: %s\n",i,errno,strerror(errno));
   fclose(fp1); 
   return 3;
  }
 }
 //-----close file----------------------------------------------
 ret=fclose(fp1);
 if(ret<0)
 {
  last_errno=errno;
  printf("savelines-4 Error %d: %s\n",errno,strerror(errno));
  return 4;
 }
 return 0;
}
 
//_____remove \r \n from end of line, set new end of string_____
int remove_lfcr(char *line_)
{
 char* p=strrchr(line_,'\0');
 p--;
 while(*p=='\r' || *p=='\n') { *p='\0'; p--; }
 return 0;
}
