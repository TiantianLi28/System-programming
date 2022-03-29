//
//  my_malloc-driver.c
//  Lab1: Malloc
//


#include "my_malloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, const char * argv[])
{
   
    
    void* add1= my_malloc(100);    
    printf("firt malloc at %p\n",add1);

    void* add2=my_malloc(300);
    printf("second malloc at %p\n", add2);

    my_free(add1);
    
    coalesce_free_list();

    char ** arr;

    arr=my_malloc(100*sizeof(char *));
    for (int r=0;r<100;r++){
        arr[r]=my_malloc(10*sizeof(char));
        printf("address of a[%d] is %p\n", r, arr[r]);
    }
        
   
    return 0;
}
