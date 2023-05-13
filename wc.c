#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <dirent.h>

int main(void) {
    int pfd[2];
    int pid;
    int unused, legitness;

    if(pipe(pfd) < 0) {
        perror("Pipe creation error!\n");
        exit(1);
    }

    if((pid = fork()) < 0) {
        perror("Child process creation error!\n");
        exit(1);
    }

    // child process
    if(pid == 0) {
        close(pfd[0]); // close read descriptor; child will write into pipe

        dup2(pfd[1], 1); // redirect standard output to pipe write end
        close(pfd[1]); 
        execlp("wc", "wc","-c", NULL); // execute wc command
        perror("Exec error!\n"); // print error message if execution fails

        close(pfd[1]);
        
        exit(1);
    } else { // parent process
        close(pfd[1]); // close write descriptor; parent will read from pipe
        printf("Lines from pipe: \n");
        char buffer[1024] = "", buff;
        int bytes_read;
        while((bytes_read=(read(pfd[0], buffer, sizeof(buffer))))>0){
            //strcat(buffer,buff);
             write(1, buffer, bytes_read);
        }


        close(pfd[0]);
        wait(NULL);
        exit(1);
    }

    return 0;

}