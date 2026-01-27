#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <pid>\n",a); exit(1);}
static int isnum(const char*s){for(;*s;s++) if(!isdigit(*s)) return 0; return 1;}
int main(int c,char**v){
 if(c!=2||!isnum(v[1])) usage(v[0]);
 char path[256]; // path buffer
 FILE *fp; // file pointer

 //Path to the status file
 snprintf(path, sizeof(path), "/proc/%s/stat", v[1]);

 fp = fopen(path, "r"); // open stat file
 if(!fp) {
   DIE("fopen stat"); // error opening file message
 }
 int pid, ppid; // parent pid
 char comm[256],state; // command name and state
 long utime,stime; // cpu times

 // Read the fields needed from the stat file like pid, comm, state, ppid, utime, stime
 if(fscanf(fp, "%d,%255s %c %d", &pid, comm, &state, &ppid) != 4) {
   DIE_MSG("fscanf stat failed to read");
 }

 for (int i = 0; i<9; i++) fscanf(fp, "%*s"); // skip to utime
 
 if (fscanf(fp, "%ld %ld", &utime, &stime) !=2){
   DIE_MSG("failed to read CPU time");
 }
fclose(fp);

}