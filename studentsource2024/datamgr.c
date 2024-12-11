/**
 * \author tolga kuntman
 */

#include "datamgr.h"
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "lib/dplist.h"
#include <assert.h>
#include "sbuffer.h"

//global variables
dplist_t *list;

//callback functions

void *element_copy(void *element)
{
    sensor_node_data_t *copy = malloc(sizeof(sensor_node_data_t));
    //char *new_name;
    copy->sensor_id = ((sensor_data_t *)element)->id;
    copy->last_modified=((sensor_node_data_t *)element)->last_modified;
    copy->next_write_index=((sensor_node_data_t *)element)->next_write_index;
    copy->running_avg=((sensor_node_data_t *)element)->running_avg;
    copy->room_id=((sensor_node_data_t *)element)->room_id;
    copy->reading_count=0;
    for(int i = 0; i < RUN_AVG_LENGTH; i++)    
        copy->past_readings[i] = ((sensor_node_data_t *)element)->past_readings[i];
    return (void *)copy;
}

void element_free(void **element)
{
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((sensor_node_data_t *)x)->sensor_id < ((sensor_node_data_t *)y)->sensor_id) ? -1
    : (((sensor_node_data_t *)x)->sensor_id == ((sensor_node_data_t *)y)->sensor_id) ? 0
    : 1);
}

void add_reading(sensor_node_data_t *sensor, double value){
    int write_index=(sensor->next_write_index);
    sensor->past_readings[write_index]=value;
    sensor->next_write_index=(write_index+1)%RUN_AVG_LENGTH;
    if (sensor->reading_count < RUN_AVG_LENGTH) {
    sensor->reading_count++;
    }
}


/**
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them. 
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 *  \param fp_sensor_map file pointer to the map file
 *  \param fp_sensor_data file pointer to the binary data file
 */

void datamgr_parse_sensor_files(FILE *fp_sensor_map, sbuffer_t *sbuf) {
    uint16_t room_id, sensor_id;
    list=dpl_create(element_copy,element_free,element_compare);
    if(list==NULL) {
        datamgr_free();
        ERROR_HANDLER(true,"error dplist is not allocated");
        }
    sensor_node_data_t sensor_input;
    while (fscanf(fp_sensor_map, "%hd %hd",&room_id, &sensor_id)==2)
    {
        sensor_input.sensor_id=sensor_id;
        sensor_input.room_id=room_id;
        for(int i = 0; i < RUN_AVG_LENGTH; i++) {
            sensor_input.past_readings[i] = 0;
        }
        sensor_input.next_write_index=0;
        sensor_input.last_modified=0;
        sensor_input.reading_count=0;
        sensor_input.running_avg=0;
        list=dpl_insert_at_index(list,&sensor_input,INT8_MAX,true);
        // printf("%hd %hd\n", sensor_input->sensor_id,sensor_input->room_id);
    }
    sensor_value_t temperature;
    //sensor_ts_t timestamp;
    sensor_node_data_t temp_sensor;
    sensor_data_t data;
    while (sbuffer_remove(sbuf,&data,0)!=SBUFFER_NO_DATA) {
        temp_sensor.sensor_id=data.id;
        temperature=data.value;
        //timestamp=data.ts;
        int index = dpl_get_index_of_element(list, &temp_sensor);
        if(index==-1)  {
            char error_msg[READ_MSG_LENGTH] = {[0 ... READ_MSG_LENGTH-1] = '\0'};
            sprintf(error_msg,"Log error: Sensor read has an invalid id %hd",
            temp_sensor.sensor_id);
            write_to_log_process(error_msg);
            continue;
        }
        sensor_node_data_t *sensor = (sensor_node_data_t *)dpl_get_element_at_index(list, index);
        add_reading(sensor,temperature);
        if(sensor->reading_count==RUN_AVG_LENGTH){
            sensor->running_avg=0;
            for(int j = 0; j < sensor->reading_count; j++) sensor->running_avg += sensor->past_readings[j]/(sensor->reading_count);
            if (sensor->running_avg < SET_MIN_TEMP) {
               fprintf(stderr, "Log error: Room %hd is too cold with average temperature %f°C measured by sensor %hd", sensor->room_id, sensor->running_avg,sensor->sensor_id);
               char error_msg[READ_MSG_LENGTH] = {[0 ... READ_MSG_LENGTH-1] = '\0'};
                sprintf(error_msg,"Log error: Room %hd is too cold with average temperature %f°C measured by sensor %hd", 
                sensor->room_id, sensor->running_avg,sensor->sensor_id);
                write_to_log_process(error_msg);
            } else if (sensor->running_avg > SET_MAX_TEMP) {
                char error_msg[READ_MSG_LENGTH] = {[0 ... READ_MSG_LENGTH-1] = '\0'};
                sprintf(error_msg,"Log error: Room %hd is too hot with average temperature %f°C measured by sensor %hd", 
                sensor->room_id, sensor->running_avg,sensor->sensor_id);
                write_to_log_process(error_msg);
            }
        }
        
        // printf("%i,%i,%f,%i,%f\n",sensor_id,0,temperature,sensor->reading_count,sensor->running_avg);
    }

}

/**
 * This method should be called to clean up the datamgr, and to free all used memory. 
 * After this, any call to datamgr_get_room_id, datamgr_get_avg, datamgr_get_last_modified or datamgr_get_total_sensors will not return a valid result
 */
void datamgr_free() {
    dpl_free(&list,true);
}

/**
 * Gets the room ID for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the corresponding room id
 */
uint16_t datamgr_get_room_id(sensor_id_t sensor_id) {
    sensor_node_data_t temp_sensor;
    temp_sensor.sensor_id = sensor_id;
    int index = dpl_get_index_of_element(list, &temp_sensor);
    if (index == -1) {
        ERROR_HANDLER(true, "room_id couldn't be found!!!!!!!!");
    }
    sensor_node_data_t *retrieved_sensor = (sensor_node_data_t *)dpl_get_element_at_index(list, index);
    return retrieved_sensor->room_id;
}


/**
 * Gets the running AVG of a certain sensor ID (if less then RUN_AVG_LENGTH measurements are recorded the avg is 0)
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the running AVG of the given sensor
 */
sensor_value_t datamgr_get_avg(sensor_id_t sensor_id) {
    sensor_node_data_t temp_sensor;
    temp_sensor.sensor_id = sensor_id;
    int index = dpl_get_index_of_element(list, &temp_sensor);
    if (index == -1) {
        ERROR_HANDLER(true, "sensor_id couldn't be found for running avg search!!!!!!!!");
    }
    sensor_node_data_t *retrieved_sensor = (sensor_node_data_t *)dpl_get_element_at_index(list, index);
    return retrieved_sensor->running_avg;
}


/**
 * Returns the time of the last reading for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the last modified timestamp for the given sensor
 */
time_t datamgr_get_last_modified(sensor_id_t sensor_id) {
    sensor_node_data_t temp_sensor;
    temp_sensor.sensor_id = sensor_id;
    int index = dpl_get_index_of_element(list, &temp_sensor); 
    if (index == -1) {
        ERROR_HANDLER(true, "sensor_id couldn't be found for last modified search!!!!!!!!");
    }
    sensor_node_data_t *retrieved_sensor = (sensor_node_data_t *)dpl_get_element_at_index(list, index);
    return retrieved_sensor->last_modified;
}


/**
 * Return the total amount of unique sensor ID's recorded by the datamgr
 * \return the total amount of sensors
 */
int datamgr_get_total_sensors() {
    return dpl_size(list);
}
