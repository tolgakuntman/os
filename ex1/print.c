#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define LIMIT 50


int main(){
    char first[LIMIT], second[LIMIT], str[2*LIMIT],str2[2*LIMIT];
    int i=0,j=0,year;
    printf("Enter your first name: ");
    scanf("%s", first);

    printf("Enter your second name: ");
    scanf("%s", second);
    printf("Enter your year: ");
    scanf("%d",&year);
    while(first[i]!='\0'){
        str[i]=first[i];
        i++;
    }
    str[i++]=' ';
    while(second[j]!='\0'){
        str[i]=('A'-'a')+second[j];
        j++;
        i++;
    }
    str[i]='\0';
    printf("%s\n",str);
    printf("comparing str and second: %d\n",strcmp(str,second));
    int print_value=snprintf(str2,LIMIT,"%s %s %d",first,second,year);
    printf("second string: %s\n",str2);
    printf("total number of char: %d\n",print_value);
    printf("Enter everything: ");
    char input[LIMIT];
    scanf("%s", input);
    sscanf(input,"%s %s %d",first,second, &year);
    printf("new name: %s\n",first);
    printf("new 2nd name: %s\n",second);
    printf("new year: %d\n",year);
    return 0;
}

