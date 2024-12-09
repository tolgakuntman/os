#include "sensor_db.h"

bool is_file_open=false;

FILE * open_db(char * filename, bool append){
    const char *mode = append ? "w" : "a";
    FILE *file= fopen(filename, mode);
    if(file==NULL){
        return NULL;
    }
    is_file_open=true;
    return file;
}
int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
    if(!is_file_open){
        return -1;
    }
    int res=fprintf(f,"%d,%lf,%ld\n", id, value, ts);
    fflush(f);
    return res<0 ? -1:0;
}
int close_db(FILE * f){
    if (!is_file_open)
    {
        return -1;
    }
    return fclose(f);
}