#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define FREQ 1
#define MIN_TEMP -10
#define MAX_TEMP 35

int main(){
    while(1){
	srand(time(0)); // let's set the seed with unix timestamp
	float n = MIN_TEMP + (float)rand() / RAND_MAX * (MAX_TEMP - MIN_TEMP);
	printf("temp: %1.2f\t", n);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("at: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sleep(FREQ);
    }
}
