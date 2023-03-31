#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    if(argc == 1) {
        printf("Not enough command line arguments.\n");
        return 1;
    }

    for(int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }

    printf("\n");

    return 0;
}