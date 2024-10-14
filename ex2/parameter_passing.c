#include <stdio.h>
#include <stdlib.h>

void swap_pointers(int *a,int *b);
void swap_pointers(int *a,int *b){
    int dummy=*a;
    *a=*b;
    *b=dummy;
}
int main(){
    int a;
    int b;
    printf("Plug in values: ");
    scanf("%d %d", &a, &b);
    swap_pointers(&a,&b);
    printf("swapped values: %d,%d\n",a,b);
    return 0;
}