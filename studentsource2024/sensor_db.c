#include "sensor_db.h"

bool is_file_open=false;

FILE * open_db(char * filename, bool append){
    const char *mode = append ? "a" : "w+";
    FILE *file= fopen(filename, mode);
    if(file==NULL){
        printf("I/O Error: failed to open DB file.\n");
        write_to_log_process("DB file failed to open.");
        return NULL;
    }
    write_to_log_process("A new csv file is created or an existing file has been opened.");
    is_file_open=true;
    return file;
}
int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
    if(!is_file_open){
        printf("DB Error: Insertion failed, db is not yet open.\n");
        return -1;
    }
    long res=fprintf(f,"%d,%lf,%ld\n", id, value, ts);
    if(res<-1){
        printf("I/O Error: failed to write sensor data.\n");
        write_to_log_process("An error occurred when writing to the csv file.");
    }else{
        char inserted_msg[READ_MSG_LENGTH] = {[0 ... READ_MSG_LENGTH-1] = '\0'};
        sprintf(inserted_msg, "Data insertion from sensor with id %d succeeded", id);
        write_to_log_process(inserted_msg);
    }
    fflush(f);
    return res<0 ? -1:0;
}
int close_db(FILE * f){
    if (!is_file_open)
    {
        printf("DB Error: DB cannot be closed, it hasn't been opened.\n");
        return -1;
    }
    is_file_open=false;
    int resp=fclose(f);
    if(resp<0){
        printf("I/O Error:Failed to close db file.\n");
        write_to_log_process("DB file failed to close.");
    }else{
        write_to_log_process("DB file normally closed.");
    }
    return resp < 0 ? -1 : 0;
}