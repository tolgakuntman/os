#include "logger.h"
#include "config.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define READ_MSG_LENGTH 70
#define FORMATTED_TIME_LENGTH 50

int fd[2];
//fd[0]  read
//fd[1]  write
char buf[80];
pid_t pid;
bool is_log_open=false;

int create_log_process(){
    if(is_log_open) return 0;
    is_log_open=true;
    if(pipe(fd)==-1){
        return -1;
    }
    pid=fork();
    if(pid<-1) return -1;
    if(pid==0){
        int counter=0;
        FILE *log_file=fopen("gateway.log","a");
        if(log_file==NULL) return -1;
        close(fd[1]);
        while(true){
            char msg[READ_MSG_LENGTH];
            long nr_read=read(fd[0],msg,READ_MSG_LENGTH);
            if(nr_read==0){//eof returns to 0, read end for the child is closed by the parent
                close(fd[0]);//close read end
                fclose(log_file);
                exit(0);
            }

            //get the time and format it in a requested way 
            char formatted_time[FORMATTED_TIME_LENGTH];
            time_t ts = time(NULL);
            //%c	Date and time representation	Sun Aug 19 02:56:02 2012
            strftime(formatted_time, FORMATTED_TIME_LENGTH, "%c", localtime(&ts));
            
            //write to log
            fprintf(log_file,"%d - %s - %s\n", counter, formatted_time, msg);
            counter++;
            fflush(log_file);
        }
    }
    else{
        close(fd[0]);
    }
    return 0;
}

int write_to_log_process(char *msg){
    if(!is_log_open){
        return -1;
    }
    if(write(fd[1],msg,READ_MSG_LENGTH)==-1){
        return -1;
    }
    return 0;
}
int end_log_process(){
    if(!is_log_open){
        return -1;
    }
    int res=close(fd[1]);
    wait(NULL);
    is_log_open=false;
    return res;
}