#define _POSIX_C_SOURCE 200809L
#include "common.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <cmd> [args]\n",a); exit(1);}
static double d(struct timespec a, struct timespec b){
 return (b.tv_sec-a.tv_sec)+(b.tv_nsec-a.tv_nsec)/1e9;}
int main(int c,char**v){
    if (c <2) usage(v[0]); // check for min arguments
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC, &start); // get start time
    pid_t pid = fork();
    if(pid <0) DIE("fork"); // fork error handling
    if(pid ==0){ // child process
        execvp(v[1],&v[1]);
        DIE("execvp");
    }
    int status;
    if(waitpid(pid,&status, 0)<0) // wait for child process
        DIE("waitpid");
    clock_gettime(CLOCK_MONOTONIC, &end); // get end time
    printf("Child PID: %d\n", pid);
    
    if(WIFEXITED(status)){ // check if exited 
        printf("exit=%d\n", WEXITSTATUS(status)); // print exit code
    } else if (WIFSIGNALED(status)){
        printf("Terminated by signal: %d\n", WTERMSIG(status)); // print signal
    }
    printf("elapsed:%.3f\n", d(start, end)); // print elapsed time

 return 0;
}
