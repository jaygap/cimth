#include <stdio.h>
#include <string.h>

// TODO
//
// argument parser
// implement one algorithm single-threaded, then multi-threaded, then hardware accelerated
// repeat for rest

int main(int argc, char **argv){
    if (argc <= 1){
        printf("Not enough arguments given.\n");
        return -1;
    }

    if (strcmp(argv[1], "-m") == 0){
        printf("First arguments should define the manipulation mode.\n");
        return -1;
    }

    if (argc == 2){
        printf("Expected a mode to be given");
        return -1;
    }

    return 0;
}

