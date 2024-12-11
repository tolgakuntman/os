#include "config.h"
#include "sbuffer.h"

#include "connmgr.h"
#include "datamgr.h"
#include "sensor_db.h"

#include "pthread.h"

#define FORMATTED_TIME_LENGTH 50

int fd[2];
//fd[0]  read
//fd[1]  write
char buf[80];
pid_t pid;
bool is_log_open=false;
pthread_mutex_t pipe_lock;

int create_log_process(){
    if(is_log_open) return 0;
    is_log_open=true;
    if(pipe(fd)==-1){
        return -1;
    }
    pid=fork();
    if(pid<0) return -1;
    if(pid==0){
        int counter=0;
        FILE *log_file=fopen("gateway.log","w+");
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

        pthread_mutex_init(&pipe_lock,NULL);
    }
    return 0;
}

int write_to_log_process(char *msg){
    if(!is_log_open){
        return -1;
    }
    pthread_mutex_lock(&pipe_lock);
    if(write(fd[1],msg,READ_MSG_LENGTH)==-1){
        pthread_mutex_unlock(&pipe_lock);
        return -1;
    }
    pthread_mutex_unlock(&pipe_lock);
    return 0;
}
int end_log_process(){
    if(!is_log_open){
        return -1;
    }
    int res=close(fd[1]);
    pthread_mutex_destroy(&pipe_lock);
    wait(NULL);
    is_log_open=false;
    return res;
}

void *connmgr(void *args){
    arg_t *arg=args;
    arg_t *arg_client=(arg_t *)malloc(sizeof(arg_t));
    arg_client->arg1=arg->arg1;//PORT
    arg_client->arg2=arg->arg2;//MAX_CONN
    arg_client->arg3=arg->arg3;
    pthread_t server;
    pthread_create(&server,NULL,&wait_for_clients,arg_client);
    pthread_join(server,NULL);
    free(arg_client);
    return NULL;
}

void *storage_manager(void *args){
    sbuffer_t *sbuf=((sbuffer_t *)args);
    FILE *f = open_db("sensor_db.csv", false);
    sensor_data_t *data=(sensor_data_t *)malloc(sizeof(sensor_data_t));
    while (sbuffer_remove(sbuf,data,1)!=SBUFFER_NO_DATA)
    {
        insert_sensor(f,data->id,data->value,data->ts);
    }
    close_db(f);
    free(data);
    return NULL;
}

void *data_manager(void *args){
    sbuffer_t *sbuf=((sbuffer_t *)args);
    FILE *map = fopen("room_sensor.map", "r");
    datamgr_parse_sensor_files(map, sbuf);
    fclose(map);
    datamgr_free();
    return NULL;
}

int main(int argc, char *argv[])
{
    create_log_process();
    if(argc < 3) {
    	write_to_log_process("Please provide correct and enough arguments");
        end_log_process();
    	return -1;
    }
    sbuffer_t *sbuf;
    sbuffer_init(&sbuf);
    pthread_t connmgr_thread;
    arg_t *arg_connmng=(arg_t *)malloc(sizeof(arg_t));
    int PORT=atoi(argv[1]);
    int MAX_CONN=atoi(argv[2]);
    arg_connmng->arg1=&PORT;//PORT
    arg_connmng->arg2=&MAX_CONN;//MAX_CONN
    arg_connmng->arg3=sbuf;
    pthread_create(&connmgr_thread,NULL,&connmgr,arg_connmng);

    pthread_t storage_manager_thread;
    pthread_create(&storage_manager_thread,NULL,&storage_manager,sbuf);
    
    pthread_t data_manager_thread;
    pthread_create(&data_manager_thread,NULL,&data_manager,sbuf);

    pthread_join(connmgr_thread,NULL);
    pthread_join(storage_manager_thread,NULL);
    pthread_join(data_manager_thread,NULL);
    printf("threads done\n");
    fflush(stdout);
    sbuffer_free(&sbuf);
    free(arg_connmng);
    end_log_process();
    return 0;
}