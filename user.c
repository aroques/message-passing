#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "global_constants.h"
#include "helpers.h"


int get_duration();

int main (int argc, char *argv[]) {
    srand(time(NULL)); // Only seed once
   
    int duration = get_duration();
    printf("hello world!\n");

    // Need access to shared clock
    //

    return 0;  
}

int get_duration() {
    return rand() % USER_RUNTIME;
}