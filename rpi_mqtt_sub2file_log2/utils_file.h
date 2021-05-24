/* utils_file.h, 13.8.2014-21.2.2019, Karl Hartinger          */
/* 21.2.2019: version for linux (Raspberry Pi) or Win         */
#ifndef _FILE_UTILS_H_
 #define _FILE_UTILS_H_
 #include <stdlib.h>              // system
 #ifdef  __cplusplus
  extern "C" {
 #endif
 #include <stdio.h>               // printf
 #include <string.h>              // sprintf
 #include <errno.h>               // errno, strerror

 #define LINUX_                  1     // 1=linux, 0=win
 #define EOL                  "\n"     //

#ifndef _FILE_TEMP_
 #define _FILE_TEMP_     "zzz.log"     //
#endif

 #ifdef FILENAME_MAX
  #undef FILENAME_MAX
 #endif
 #define FILENAME_MAX         4096     // LINUX-Default: 4096 !!!

#ifndef LINE_LEN_MAX
 #define LINE_LEN_MAX        254  // 896 date;cmd;par;ttl;ok;com
#endif

 //=====Special data types======================================
 typedef char type_line[LINE_LEN_MAX+1];
 extern type_line last_error;
 extern int last_errno;

 //_____does file exist?________________________________________
 // returns 1=yes, 0=no
 int isfile(char *filename_);
 //_____copy file_______________________________________________
 // returns 0=OK, <>0 on error
 int copyfile(char *filenamefrom_, char *filenameto_);
 //_____rename file_____________________________________________
 // returns 0=OK, <>0 on error
 int renamefile(char *filenameold_, char *filenamenew_);
 //_____delete file_____________________________________________
 // returns 0=OK, <>0 on error
 int deletefile(char *filename_);
 //_____count files in dir_______________________________________
 // pathfilename e.g. ~/*.*
 // return: number of files or -1 on error
 int countfilel(char *pathfilename_);

 //_____writes a line (string) in a file________________________
 // old values will be deleted
 // returns 0=OK, 1 on error
 int writeline(char *filename_, char *line_);
 //_____insert a new line in log file___________________________
 // returns 0=OK, 1 on error
 int appendline(char *filename_, char *line_);
 //_____insert a new FIRST line in log file_____________________
  // returns 0=OK, 1,2 or 3 on error
 int insertfirstline(char *filename_, char *line_);
 //_____get first line of file__________________________________
 // returns 0=OK, 1 on error
 int readfirstline(char *filename_, char *line_);
 //_____get last line of file___________________________________
 // returns 0=OK, 1 on error
 int readlastline(char *filename_, char *line_);
 //_____get last line with text__________________________________
 // returns 0=OK, 1 on error
 int readlasttextline(char *filename_, char *line_);
 //_____show content of (text) file_____________________________
 // returns 0=OK, 1 on error
 int showfile(char *filename_);
 //_____save a line to a file___________________________________
 // type "w" makes a new file, "a" appends to a file
 // returns 0=OK, 1 on error
 int saveline(char *filename_, char* type, char *line_);
 //_____save lines to a file____________________________________
 // type "w" makes a new file, "a" appends to a file
 // returns 0=OK, 1 on error
 int savelines(char *filename_, char* type_, int nr_, type_line lines_[]);
//_____replace a line beginning with startext___________________
int replaceline(char* filename_, char* starttext, char* replacetext);

//_____remove \r \n from end of line, set new end of string____
 int remove_lfcr(char *line_);

 //_____remove blank(s) from begin of line______________________
 int remove_leadingblank(char *line_);

 //_____remove blank(s) from end of line________________________
 int remove_trailingblank(char *line_);


 #ifdef  __cplusplus
 }
 #endif
#endif //_FILE_UTILS_H_
