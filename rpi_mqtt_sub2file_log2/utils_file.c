/* utils_file.c, 13.8.2014-21.2.2019, Karl Hartinger          */
/* 21.2.2019: version for linux (Raspberry Pi) or Win         */
#include "utils_file.h"

//=====Special data types=======================================
type_line last_error;
int last_errno;

//_____does file exist?_________________________________________
// returns 1=yes, 0=no
int isfile(char *filename_)
{
 FILE *fp;
 remove_lfcr(filename_);
 if( (fp=fopen(filename_, "r")) == NULL) return 0;
 fclose(fp);
 return 1;
}

//_____copy file________________________________________________
// returns 0=OK, <>0 on error
int copyfile(char *filenamefrom_, char *filenameto_)
{
 char temp[2*FILENAME_MAX+6];
 remove_lfcr(filenamefrom_);
 remove_lfcr(filenameto_);
#ifdef LINUX_
 sprintf(temp, "cp %s %s", filenamefrom_, filenameto_);
#else
 sprintf(temp, "copy %s %s", filenamefrom_, filenameto_);
#endif
 return(system(temp));
}

//_____rename file______________________________________________
// old -> new name
// returns 0=OK, <>0 on error
int renamefile(char *filenameold_, char *filenamenew_)
{
 char temp[2*FILENAME_MAX+10];
 remove_lfcr(filenameold_);
 remove_lfcr(filenamenew_);
#ifdef LINUX_
 sprintf(temp, "mv %s %s", filenameold_, filenamenew_);
#else
 sprintf(temp, "rename %s %s", filenameold_, filenamenew_);
#endif
 return(system(temp));
}

//_____delete file______________________________________________
// returns 0=OK, <>0 on error
int deletefile(char *filename_)
{
 char temp[FILENAME_MAX+4];
 remove_lfcr(filename_);
 if(isfile(filename_)==0) return 1;
 #ifdef LINUX_
  sprintf(temp, "rm %s", filename_);
 #else
  sprintf(temp, "del %s", filename_);
 #endif
  return(system(temp));
}

//_____count files in dir_______________________________________
// pathfilename e.g. ~/*.*
// return: number of files or -1 on error
int countfilel(char *pathfilename_)
{
 FILE *fp=NULL;
 char temp[FILENAME_MAX+20];
 char sline[LINE_LEN_MAX];
 remove_lfcr(pathfilename_);
#ifdef LINUX_
 sprintf(temp,"ls -a -1 %s | wc -l",pathfilename_);
 if((fp=popen(temp, "r"))==NULL) return -1;
 fgets(sline, LINE_LEN_MAX, fp);
 pclose(fp);
 return atoi(sline);
#else
 return -1;
#endif
}

//_____writes a line (string) in a file_________________________
// old values will be deleted
// returns 0=OK, 1 on error
int writeline(char *filename_, char *line_)
{
 char w[]="w";
 return (saveline(filename_, w, line_));
}

//_____insert (append) a new line in log file___________________
// returns 0=OK, 1 on error
int appendline(char *filename_, char *line_)
{
 char a[]="a";
 return (saveline(filename_, a, line_));
}

//_____insert a new first line in log file______________________
// returns 0=OK, 1,2 or 3 on error
int insertfirstline(char *filename_, char *line_)
{
 FILE *fp1, *fp2;
 type_line line1_;
 int ret;
 char temp[]="temp.log";
 char w[]="w";
 char r[]="r";
 char a[]="a";
 //-----delete temp file----------------------------------------
 if(isfile(temp)) deletefile(temp);
 //-----file does not exist yet---------------------------------
 if(!isfile(filename_))
 {
  ret=saveline(filename_, w, line_);
  return ret;
 }
 //-----copy old file to temp.log-------------------------------
 ret=copyfile(filename_,temp);
 if(ret!=0) return ret;
 ret=saveline(filename_, w, line_);
 if(ret!=0) return ret;
 //-----open file-----------------------------------------------
 fp1=fopen(temp, r);
 if( fp1 == NULL)
 {
  last_errno=errno;
  sprintf(last_error,"Fehler 1 - insertfirstline: %d %s\n",errno,strerror(errno));
  return 1;
 }
 fp2=fopen(filename_, a);
 if( fp2 == NULL)
 {
  last_errno=errno;
  sprintf(last_error,"Fehler 2 - insertfirstline: %d %s\n",errno,strerror(errno));
  fclose(fp1);
  deletefile(temp);
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
   deletefile(temp);
   return 3;
  }
 }
 fclose(fp2);
 fclose(fp1);
 deletefile(temp);
 return ret;
}

//_____get first line___________________________________________
// returns 0=OK, 1 on error
int readfirstline(char *filename_, char *line_)
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
int readlastline(char *filename_, char *line_)
{
 FILE *fp;
 if( (fp=fopen(filename_, "r")) == NULL) return 1;
 while( fgets(line_, LINE_LEN_MAX, fp) != NULL) ;
 remove_lfcr(line_);
 fclose(fp);
 return 0;
}

//_____get last line with text__________________________________
// returns 0=OK, 1 on error
int readlasttextline(char *filename_, char *line_)
{
 FILE *fp;
 char temp[LINE_LEN_MAX];
 line_[0]=0;
 if( (fp=fopen(filename_, "r")) == NULL) return 1;
 while( fgets(temp, LINE_LEN_MAX, fp) != NULL)
 {
  remove_lfcr(temp);
  if(strlen(temp)>0) strcpy(line_,temp);
 }
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
 remove_lfcr(filename_);
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

//_____replace a line beginning with startext___________________
int replaceline(char* filename_, char* starttext, char* replacetext)
{
 FILE *fp1, *fp2;
 char line_[LINE_LEN_MAX];
 char temp[4+2*FILENAME_MAX];
 int ret=0;
// char w[]="w";
 char r[]="r";
 char a[]="a";
 int len_;
 //-----delete temp file----------------------------------------
 strcpy(temp,_FILE_TEMP_);
 if(isfile(temp)) deletefile(temp);
 //-----copy old file to _FILE_TEMP_---------------------------
 ret=renamefile(filename_, temp);
 if(ret!=0) return ret;
 len_=strlen(starttext);
 //-----rename file---------------------------------------------
 fp1=fopen(temp, r);
 if( fp1 == NULL)
 {
  last_errno=errno;
  sprintf(last_error,"Fehler 1 - insertfirstline: %d %s\n",errno,strerror(errno));
  return 1;
 }
 fp2=fopen(filename_, a);
 if( fp2 == NULL)
 {
  last_errno=errno;
  sprintf(last_error,"Fehler 2 - insertfirstline: %d %s\n",errno,strerror(errno));
  fclose(fp1);
  deletefile(temp);
  return 2;
 }
 //-----copy/replace lines--------------------------------------
 while( fgets(line_, LINE_LEN_MAX, fp1) != NULL)
 {
  if(strncmp(line_,starttext,len_)==0) {
   sprintf(line_,"%s%s%s",starttext,replacetext,EOL);
  }
  ret=fputs(line_, fp2);
  if(ret<0)
  {
   last_errno=errno;
   sprintf(last_error,"Fehler 3 - insertfirstline: %d %s\n",errno,strerror(errno));
   fclose(fp2);
   fclose(fp1);
   deletefile(temp);
   return 3;
  }
  else ret=0;
 }
 fclose(fp2);
 fclose(fp1);
 deletefile(temp);
 return ret;
}

//_____remove \r \n from end of line, set new end of string_____
int remove_lfcr(char *line_)
{
 char* p=strrchr(line_,'\0');
 p--;
 while(*p=='\r' || *p=='\n') { *p='\0'; p--; }
 return 0;
}

//_____remove blank(s) from begin of line_______________________
int remove_leadingblank(char *line_)
{
 char *p=line_;
 while(*p==' ') p++;
 strcpy(line_, p);
 return 0;
}

//_____remove blank(s) from end of line_________________________
int remove_trailingblank(char *line_)
{
 char* p=strrchr(line_,'\0');
 p--;
 while(*p==' ') { *p='\0'; p--; }
 return 0;
}
