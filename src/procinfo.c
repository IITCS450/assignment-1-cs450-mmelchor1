#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <pid>\n",a); exit(1);}
static int isnum(const char*s){for(;*s;s++) if(!isdigit(*s)) return 0; return 1;}
int main(int c,char**v){
 if(c!=2||!isnum(v[1])) usage(v[0]);
 char path[256];
 char line[256];
 FILE *fp;
 //Path to the status file
 snprintf(path, sizeof(path), "/proc/%s", v[1]);

 fp = fopen(path, "r");
 if(!fp) {
    perror("Error opening status file");
    return 1;
 }
 while(fgets(line,sizeof(line), fp)) {
    if(strncmp(line,"PPid:", 5) == 0) {
        printf("PPID: %s", line + 5);
        break;
    }
 }
 while(fgets(line,sizeof(line), fp)) {
    if(strncmp(line,"VmRSS:", 6) == 0) {
        printf("VmRSS: %s", line + 6);
        break;
    }
 }
 fclose(fp);
}
