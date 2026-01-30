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
 if(fscanf(fp, "%d %255s %c %d", &pid, comm, &state, &ppid) != 4) {
   DIE_MSG("fscanf stat failed to read");
 }

 for (int i = 0; i<9; i++) fscanf(fp, "%*s"); // skip to utime
 
 if (fscanf(fp, "%ld %ld", &utime, &stime) !=2){
   DIE_MSG("failed to read CPU time");
 }
  fclose(fp);

  // calculate cpu time in seconds
  long ticks = sysconf(_SC_CLK_TCK);
  double cpu_time = (double)(utime + stime)/ticks;

  snprintf(path, sizeof(path), "/proc/%s/cmdline",v[1]); // path to cmdline file
  fp = fopen(path, "r");
  if(!fp){
    DIE("fopen cmdline");
  }
  char cmdline[1024];
  size_t n = fread(cmdline, 1, sizeof(cmdline)-1, fp); // read file 
  fclose(fp);

  if(n ==0)
    strcpy(cmdline, "[empty]"); // handle empty cmdline
  else {
    for (size_t i=0; i<n-1; i++)
      if(cmdline[i] == '\0') cmdline[i] =' '; // replace nulls with space
    cmdline[n]='\0';
  }

  snprintf(path, sizeof(path), "/proc/%s/status", v[1]); // path to status file
  fp = fopen(path, "r");
  if(!fp){
    DIE("fopen status");
  }
  char line[256];
  char vmrss[64] = "VmRSS: N/A"; // default value

  // read through file to find VmRSS
  while(fgets(line, sizeof(line), fp)){
    if(strncmp(line, "VmRSS:",6)==0){
      sscanf(line,"VmRSS:%63[^\n]", vmrss);
      break;
    }
  }
  fclose(fp);

  printf("PID: %d\n",pid); // print pid
  printf("State: %c\n", state); // print state
  printf("PPID: %d\n", ppid); // print parent pid
  printf("Command Line: %s\n", cmdline); // print cmd line
  printf("CPU Time: %.2f seconds\n", cpu_time); // print cpu time
  printf("VmRSS:%s\n", vmrss); // print vmrss
  return 0; 
}